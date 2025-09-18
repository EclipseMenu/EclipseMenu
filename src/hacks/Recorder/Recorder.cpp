#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/button.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/hack/hack.hpp>
#include <modules/recorder/recorder.hpp>

#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/ShaderLayer.hpp>
#include <modules/gui/cocos/components/ToggleComponent.hpp>

#include <modules/i18n/translations.hpp>
#include <modules/recorder/DSPRecorder.hpp>

// android uses custom gd::string class
#ifdef GEODE_IS_ANDROID
#define STR(x) std::string(x)
#else
#define STR(x) x
#endif

namespace eclipse::hacks::Recorder {
    static recorder::Recorder s_recorder;

    bool levelDone = false;
    bool popupShown = false;
    bool capturing = false;

    float totalTime = 0.f;
    float afterEndTimer = 0.f;

    float extraTime = 0.f;
    float lastFrameTime = 0.f;

    cocos2d::CCSize oldDesignResolution;
    cocos2d::CCSize newDesignResolution;
    cocos2d::CCSize originalScreenScale;
    cocos2d::CCSize newScreenScale;

    void callback(std::string const& error) {
        geode::queueInMainThread([error] {
            Popup::create(i18n::get_("common.error"), error);
        });
    }

    void endPopup() {
        Popup::create(
            i18n::get_("common.info"),
            i18n::format("recorder.finished", s_recorder.getRecordingDuration()),
            i18n::get_("common.ok"),
            i18n::get_("recorder.open-folder"),
            [](bool result) {
                if (result) return;

                geode::utils::file::openFolder(geode::Mod::get()->getSaveDir() / "renders");
            }
        );
    }

    void applyWinSize() {
        if (newDesignResolution.width != 0 && newDesignResolution.height != 0) {
            auto view = utils::get<cocos2d::CCEGLView>();

            utils::get<cocos2d::CCDirector>()->m_obWinSizeInPoints = newDesignResolution;
            view->setDesignResolutionSize(
                newDesignResolution.width, newDesignResolution.height, ResolutionPolicy::kResolutionExactFit
            );
            view->m_fScaleX = newScreenScale.width;
            view->m_fScaleY = newScreenScale.height;
        }
    }

    void restoreWinSize() {
        if (oldDesignResolution.width != 0 && oldDesignResolution.height != 0) {
            auto view = utils::get<cocos2d::CCEGLView>();

            utils::get<cocos2d::CCDirector>()->m_obWinSizeInPoints = oldDesignResolution;
            view->setDesignResolutionSize(
                oldDesignResolution.width, oldDesignResolution.height, ResolutionPolicy::kResolutionExactFit
            );
            view->m_fScaleX = originalScreenScale.width;
            view->m_fScaleY = originalScreenScale.height;
        }
    }

    // UI trigger will offset some objects, which may cause issues if we record in a different resolution
    // this function will first revert the objects to their original position, then call positionUIObjects again
    // to update the positions according to the new resolution
    void fixUIObjects() {
        auto pl = utils::get<PlayLayer>();

        // reset ui object start positions
        for (auto obj : geode::cocos::CCArrayExt<GameObject*>(pl->m_objects)) {
            auto it = pl->m_uiObjectPositions.find(obj->m_uniqueID);
            if (it == pl->m_uiObjectPositions.end()) continue;

            // revert the object to its original position
            obj->setStartPos(it->second);
        }

        // refresh the ui objects
        pl->positionUIObjects();
    }

    inline void trimString(std::string& str) {
        str.erase(std::ranges::unique(str, [](char a, char b) {
            return std::isspace(a) && std::isspace(b);
        }).begin(), str.end());
    }

    void start() {
        if (!utils::get<PlayLayer>()) return;

        levelDone = false;
        popupShown = false;
        totalTime = 0.f;
        extraTime = 0.f;
        lastFrameTime = 0.f;
        afterEndTimer = 0.f;

        GJGameLevel* lvl = utils::get<PlayLayer>()->m_level;

        std::string trimmedLevelName = lvl->m_levelName;
        std::erase(trimmedLevelName, '/');
        std::erase(trimmedLevelName, '\\');
        trimString(trimmedLevelName);

        std::filesystem::path renderDirectory = geode::Mod::get()->getSaveDir() / "renders" / STR(trimmedLevelName);

        std::error_code ec;
        if (!std::filesystem::exists(renderDirectory, ec)) {
            std::filesystem::create_directories(renderDirectory, ec);
            if (ec) {
                return Popup::create(
                    i18n::get_("common.error"),
                    ec.message()
                );
            }
        }

        s_recorder.m_renderSettings.m_bitrate = static_cast<int>(config::get<float>("recorder.bitrate", 30.f)) * 1000000;
        s_recorder.m_renderSettings.m_fps = static_cast<int>(config::get<float>("recorder.fps", 60.f));
        s_recorder.m_renderSettings.m_width = config::get<int>("recorder.resolution.x", 1920);
        s_recorder.m_renderSettings.m_height = config::get<int>("recorder.resolution.y", 1080);
        s_recorder.m_renderSettings.m_codec = config::get<std::string>("recorder.codecString", "libx264");
        s_recorder.m_renderSettings.m_outputFile = renderDirectory / (fmt::format("{} - {}.mp4", trimmedLevelName, lvl->m_levelID.value()));
        s_recorder.m_renderSettings.m_hardwareAccelerationType = static_cast<ffmpeg::HardwareAccelerationType>(config::get<int>("recorder.hwType", 0));
        s_recorder.m_renderSettings.m_colorspaceFilters = config::get<std::string>("recorder.colorspace", "");
        s_recorder.m_renderSettings.m_doVerticalFlip = false;

        auto view = utils::get<cocos2d::CCEGLView>();

        oldDesignResolution = view->getDesignResolutionSize();
        float aspectRatio = static_cast<float>(s_recorder.m_renderSettings.m_width) / static_cast<float>(s_recorder.m_renderSettings.m_height);
        newDesignResolution = cocos2d::CCSize(roundf(320.f * aspectRatio), 320.f);

        originalScreenScale = cocos2d::CCSize(view->m_fScaleX, view->m_fScaleY);
        auto retinaRatio = geode::utils::getDisplayFactor();
        newScreenScale = cocos2d::CCSize(
            static_cast<float>(s_recorder.m_renderSettings.m_width) / newDesignResolution.width / retinaRatio,
            static_cast<float>(s_recorder.m_renderSettings.m_height) / newDesignResolution.height / retinaRatio
        );

        if(oldDesignResolution != newDesignResolution) {
            applyWinSize();
            fixUIObjects();
        }

        s_recorder.start();
    }

    void stop() {
        s_recorder.stop();
    }

    class $hack(InternalRecorder) {
        void init() override {}

        void lateInit() override {
            auto ffmpeg = geode::Loader::get()->getLoadedMod("eclipse.ffmpeg-api");
            if (!ffmpeg) return;

            // check if ffmpeg-api is 1.2.0 or higher
            if (ffmpeg->getVersion() < geode::VersionInfo(1, 2, 0)) {
                geode::log::warn("Internal Recorder requires ffmpeg-api 1.2.0 or higher. (Current: {})", ffmpeg->getVersion());
                return;
            }

            s_recorder.setCallback(callback);

            auto tab = gui::MenuTab::find("tab.recorder");

            tab->addButton("recorder.start")->callback(start);
            tab->addButton("recorder.stop")->callback([] {
                if (s_recorder.isRecording())
                    stop();
            });

            config::setIfEmpty("recorder.fps", 60.f);
            config::setIfEmpty("recorder.endscreen", 3.4f);
            config::setIfEmpty("recorder.bitrate", 30.f);
            config::setIfEmpty("recorder.resolution.x", 1920.f);
            config::setIfEmpty("recorder.resolution.y", 1080.f);
            config::setIfEmpty("recorder.hwType", 0);
            config::setIfEmpty("recorder.colorspace", "");

            m_codecs = recorder::Recorder::getAvailableCodecs();

            std::ranges::sort(m_codecs);

            int codecIdx = static_cast<int>(std::distance(m_codecs.begin(), std::ranges::find(m_codecs, "libx264")));

            tab->addInputFloat("recorder.framerate", "recorder.fps", 1.f, 360.f, "%.0f FPS");
            tab->addInputFloat("recorder.endscreen-duration", "recorder.endscreen", 0.f, 30.f, "%.2fs.");
            tab->addInputFloat("recorder.bitrate", 1.f, 1000.f, "%.0fmbps");
            tab->addInputInt("recorder.res-x", "recorder.resolution.x", 1, 15360);
            tab->addInputInt("recorder.res-y", "recorder.resolution.y", 1, 8640);
            tab->addToggle("recorder.hide-preview")->setDescription();

            config::setIfEmpty("recorder.codecIdx", codecIdx);

            tab->addCombo("recorder.codec", "recorder.codecIdx", m_codecs, codecIdx)->callback([&](int index) {
                config::set("recorder.codecString", m_codecs[index]);
            });

            //i honestly dont think hw is working rn, will remove this for now
            // tab->addCombo("HW Type", "recorder.hwIdx", {"None", "CUDA (Nvidia)", "D3D11 (All)"}, 0)->callback([&](int index) {
            //     switch(index) {
            //         case 0:
            //         default:
            //             config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::NONE));
            //             break;
            //         case 1:
            //             config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::CUDA));
            //             break;
            //         case 2:
            //             config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::D3D11VA));
            //             break;
            //     }
            // });

            tab->addInputText("recorder.colorspace-args", "recorder.colorspace");

            #ifdef GEODE_IS_DESKTOP
            tab->addLabel("recorder.presets");
            tab->addButton("recorder.preset.cpu")->callback([] {
                config::set<std::string>("recorder.codecString", "libx264");
            });

            tab->addButton("recorder.preset.nvidia")->callback([] {
                config::set<std::string>("recorder.codecString", "h264_nvenc");
                config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::CUDA));
            });

            tab->addButton("recorder.preset.amd")->callback([] {
                config::set<std::string>("recorder.codecString", "h264_amf");
                config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::D3D11VA));
            });
            #endif
        }

        [[nodiscard]] const char* getId() const override { return "Internal Recorder"; }

        std::vector<std::string> m_codecs;
    };

    REGISTER_HACK(InternalRecorder)

    class $modify(InternalRecorderSLHook, ShaderLayer) {
        void visit() {
            if (s_recorder.isRecording()) {
                setScaleY(-1);
                ShaderLayer::visit();
                return setScaleY(1);
            }

            ShaderLayer::visit();
        }
    };

    class $modify(InternalRecorderSchedulerHook, cocos2d::CCScheduler) {
        ENABLE_SAFE_HOOKS_ALL()

        void update(float dt) {
            if (s_recorder.isRecording()) {
                float framerate = config::get<"recorder.fps", float>(60.f);

                if (framerate < 1)
                    framerate = 1;

                float tps = utils::getTPS();

                dt = 1.f / framerate;
                dt *= framerate / tps;

                applyWinSize();
                CCScheduler::update(dt);
                restoreWinSize();

                return;
            }

            CCScheduler::update(dt);
        }
    };

    class $modify(InternalRecorderBGLHook, GJBaseGameLayer) {
        static void onModify(auto& self) {
            SAFE_SET_PRIO("GJBaseGameLayer::update", SAFE_HOOK_PRIORITY + 1);
            HOOKS_TOGGLE("recorder.hide-preview", GJBaseGameLayer, "visit");
        }

        void update(float dt) override {
            if (!s_recorder.isRecording() || m_gameState.m_currentProgress <= 0) return GJBaseGameLayer::update(dt);

            float endscreen = config::get<"recorder.endscreen", float>(5.f);
            eclipse::config::set<bool>("global.tpsbypass.toggle", true);

            if (levelDone) {
                if (afterEndTimer > endscreen && s_recorder.isRecording() && !popupShown) {
                    popupShown = true;
                    stop();
                    endPopup();
                    return GJBaseGameLayer::update(dt);
                }
                afterEndTimer += dt;
            }

            if (!s_recorder.isRecording())
                return GJBaseGameLayer::update(dt);

            float fps = config::get<"recorder.fps", float>(60.f);
            float timewarp = m_gameState.m_timeWarp;

            totalTime += dt;

            float frameDt = 1. / fps * timewarp;
            float time = totalTime + extraTime - lastFrameTime;

            DSPRecorder::get()->tryUnpause(totalTime);

            if (time >= frameDt) {
                extraTime = time - frameDt;
                lastFrameTime = totalTime;

                capturing = true;
                s_recorder.captureFrame();
                capturing = false;
            }

            GJBaseGameLayer::update(dt);
        }

        void visit() override {
            if (s_recorder.isRecording() && !capturing) return;
            GJBaseGameLayer::visit();
        }
    };

    class $modify(InternalRecorderPLHook, PlayLayer) {
        void onQuit() {
            if (s_recorder.isRecording()) stop();
            PlayLayer::onQuit();
        }

        void levelComplete() {
            PlayLayer::levelComplete();
            levelDone = true;
        }

        void resetLevel() {
            levelDone = false;
            popupShown = false;
            PlayLayer::resetLevel();
        }
    };
};
