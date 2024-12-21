#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/recorder/recorder.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/ShaderLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <regex>

// android uses custom gd::string class
#ifdef GEODE_IS_ANDROID
#define STR(x) std::string(x)
#else
#define STR(x) x
#endif

namespace eclipse::hacks::Recorder {

    static recorder::Recorder s_recorder;

    bool visiting = false;
    bool inShaderLayer = false;
    bool levelDone = false;
    bool popupShown = false;

    float totalTime = 0.f;
    float afterEndTimer = 0.f;

    double extraTime = 0.;
    double lastFrameTime = 0.;

    cocos2d::CCSize oldDesignResolution;
    cocos2d::CCSize newDesignResolution;
    cocos2d::CCSize originalScreenScale;
    cocos2d::CCSize newScreenScale;

    void endPopup() {
        Popup::create(
            i18n::get_("common.info"),
            i18n::get_("recorder.finished"),
            i18n::get_("common.ok"),
            i18n::get_("recorder.open-folder"),
            [](bool result) {
                if(result)
                    return;

                geode::utils::file::openFolder(geode::Mod::get()->getSaveDir() / "renders");
            }
        );
    }

    void applyWinSize() {
        if(newDesignResolution.width != 0 && newDesignResolution.height != 0) {
            auto view = cocos2d::CCEGLView::get();
            
            cocos2d::CCDirector::get()->m_obWinSizeInPoints = newDesignResolution;
            view->setDesignResolutionSize(newDesignResolution.width, newDesignResolution.height, ResolutionPolicy::kResolutionExactFit);
            view->m_fScaleX = newScreenScale.width;
            view->m_fScaleY = newScreenScale.height;
        }
    }

    void restoreWinSize() {
        if(oldDesignResolution.width != 0 && oldDesignResolution.height != 0) {
            auto view = cocos2d::CCEGLView::get();

            cocos2d::CCDirector::get()->m_obWinSizeInPoints = oldDesignResolution;
            view->setDesignResolutionSize(oldDesignResolution.width, oldDesignResolution.height, ResolutionPolicy::kResolutionExactFit);
            view->m_fScaleX = originalScreenScale.width;
            view->m_fScaleY = originalScreenScale.height;
        }
    }

    void start() {
        if (!PlayLayer::get()) return;

        visiting = false;
        levelDone = false;
        popupShown = false;
        totalTime = 0.f;
        extraTime = 0.;
        lastFrameTime = 0.;
        afterEndTimer = 0.f;

        GJGameLevel* lvl = PlayLayer::get()->m_level;

        std::string trimmedLevelName = lvl->m_levelName;
        trimmedLevelName.erase(std::remove(trimmedLevelName.begin(), trimmedLevelName.end(), '/'), trimmedLevelName.end());
        trimmedLevelName.erase(std::remove(trimmedLevelName.begin(), trimmedLevelName.end(), '\\'), trimmedLevelName.end());
        trimmedLevelName = std::regex_replace(trimmedLevelName, std::regex("\\s+"), " ");
        std::filesystem::path renderDirectory = geode::Mod::get()->getSaveDir() / "renders" / STR(trimmedLevelName);

        if (!std::filesystem::exists(renderDirectory))
            std::filesystem::create_directories(renderDirectory);

        s_recorder.m_renderSettings.m_bitrate = static_cast<int>(config::get<float>("recorder.bitrate", 30.f)) * 1000000;
        s_recorder.m_renderSettings.m_fps = static_cast<int>(config::get<float>("recorder.fps", 60.f));
        s_recorder.m_renderSettings.m_width = config::get<int>("recorder.resolution.x", 1920);
        s_recorder.m_renderSettings.m_height = config::get<int>("recorder.resolution.y", 1080);
        s_recorder.m_renderSettings.m_codec = config::get<std::string>("recorder.codecString", "h264");
        s_recorder.m_renderSettings.m_outputFile = renderDirectory / (fmt::format("{} - {}.mp4", trimmedLevelName, lvl->m_levelID.value()));
        s_recorder.m_renderSettings.m_hardwareAccelerationType = static_cast<ffmpeg::HardwareAccelerationType>(config::get<int>("recorder.hwType", 0));
        s_recorder.m_renderSettings.m_colorspaceFilters = config::get<std::string>("recorder.colorspace", "");

        auto view = cocos2d::CCEGLView::get();

        oldDesignResolution = view->getDesignResolutionSize();
        float aspectRatio = static_cast<float>(s_recorder.m_renderSettings.m_width) / static_cast<float>(s_recorder.m_renderSettings.m_height);
        newDesignResolution = cocos2d::CCSize(roundf(320.f * aspectRatio), 320.f);

        originalScreenScale = cocos2d::CCSize(view->m_fScaleX, view->m_fScaleY);
        newScreenScale = cocos2d::CCSize(static_cast<float>(s_recorder.m_renderSettings.m_width) / newDesignResolution.width, static_cast<float>(s_recorder.m_renderSettings.m_height) / newDesignResolution.height);

        if(oldDesignResolution != newDesignResolution)
            applyWinSize();

        s_recorder.start();
    }

    void stop() {
        s_recorder.stop();
    }

    void startAudio() {
        stop();

        FMODAudioEngine::get()->stopAllEffects();

        auto lvl = PlayLayer::get()->m_level;
        std::string trimmedLevelName = lvl->m_levelName;
        std::erase(trimmedLevelName, '/');
        std::erase(trimmedLevelName, '\\');
        trimmedLevelName = std::regex_replace(trimmedLevelName, std::regex("\\s+"), " ");
        auto renderPath = geode::Mod::get()->getSaveDir() / "renders" / STR(trimmedLevelName) / (fmt::format("{} - {}.mp4", trimmedLevelName, lvl->m_levelID.value()));

        if (!std::filesystem::exists(renderPath)) {
            geode::log::error("Render {} not found", renderPath);
            return;
        }

        levelDone = false;
        popupShown = false;
        afterEndTimer = 0.f;

        if (auto ell = PlayLayer::get()->getChildByType<EndLevelLayer>(0))
            ell->removeFromParent();

        PlayLayer::get()->stopAllActions();
        PlayLayer::get()->startGame();
        PlayLayer::get()->resetLevelFromStart();

        s_recorder.startAudio(renderPath);
    }

    void stopAudio() {
        s_recorder.stopAudio();
    }

    class InternalRecorder : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.recorder");
        }

        void lateInit() override {
            auto ffmpeg = geode::Loader::get()->getLoadedMod("eclipse.ffmpeg-api");
            if (!ffmpeg) return;
            
            auto tab = gui::MenuTab::find("tab.recorder");

            tab->addButton("recorder.start")->callback(start);
            tab->addButton("recorder.stop")->callback([] {
                if (s_recorder.isRecording())
                    stop();
                if (s_recorder.isRecordingAudio())
                    stopAudio();
            });

            config::setIfEmpty("recorder.fps", 60.f);
            config::setIfEmpty("recorder.endscreen", 3.4f);
            config::setIfEmpty("recorder.bitrate", 30.f);
            config::setIfEmpty("recorder.resolution.x", 1920.f);
            config::setIfEmpty("recorder.resolution.y", 1080.f);
            config::setIfEmpty("recorder.audio", 2);
            config::setIfEmpty("recorder.hwType", 0);
            config::setIfEmpty("recorder.colorspace", "");

            m_codecs = s_recorder.getAvailableCodecs();

            std::ranges::sort(m_codecs);

            int codecIdx = static_cast<int>(std::distance(m_codecs.begin(), std::ranges::find(m_codecs, "h264")));

            tab->addInputFloat("recorder.framerate", "recorder.fps", 1.f, 360.f, "%.0f FPS");
            tab->addInputFloat("recorder.endscreen-duration", "recorder.endscreen", 0.f, 30.f, "%.2fs.");
            tab->addInputFloat("recorder.bitrate", "recorder.bitrate", 1.f, 1000.f, "%.0fmbps");
            tab->addInputInt("recorder.res-x", "recorder.resolution.x", 1, 15360);
            tab->addInputInt("recorder.res-y", "recorder.resolution.y", 1, 8640);

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

            tab->addCombo("recorder.audio-mode", "recorder.audio", {
                i18n::get_("recorder.audio-mode.disable"),
                i18n::get_("recorder.audio-mode.ask"),
                i18n::get_("recorder.audio-mode.always")
            }, 0);

            tab->addInputText("recorder.colorspace-args", "recorder.colorspace");
            
            tab->addLabel("recorder.presets");
            tab->addButton("recorder.preset.cpu")->callback([] {
                config::set<std::string>("recorder.codecString", "h264");
            });

            tab->addButton("recorder.preset.nvidia")->callback([] {
                config::set<std::string>("recorder.codecString", "h264_nvenc");
                config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::CUDA));
            });

            tab->addButton("recorder.preset.amd")->callback([] {
                config::set<std::string>("recorder.codecString", "h264_amf");
                config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::D3D11VA));
            });
        }

        [[nodiscard]] const char* getId() const override { return "Internal Recorder"; }

        std::vector<std::string> m_codecs;
    };

    REGISTER_HACK(InternalRecorder)

    class $modify(InternalRecorderSLHook, ShaderLayer) {
        void visit() {
            inShaderLayer = true;
            ShaderLayer::visit();
            inShaderLayer = false;
        }
    };

    class $modify(InternalRecorderSchedulerHook, cocos2d::CCScheduler) {
        ENABLE_SAFE_HOOKS_ALL()

        void update(float dt) {
            if (s_recorder.isRecording()) {
                float framerate = config::get<float>("recorder.fps", 60.f);

                if (framerate < 1)
                    framerate = 1;

                dt = 1.f / framerate;

                applyWinSize();
                CCScheduler::update(dt);
                restoreWinSize();

                return;
            }

            CCScheduler::update(dt);
        }
    };

    class $modify(InternalRecorderBGLHook, GJBaseGameLayer) {
        void syncMusic() {
            //temp hardcoded
            uint32_t tps = 240;

            float songTime = (static_cast<float>(m_gameState.m_currentProgress) / tps) * 1000.f;
            songTime += m_levelSettings->m_songOffset * 1000.f;

            FMOD::Channel* audioChannel;

            for (int i = 0; i < 2; i++) {
                FMODAudioEngine::sharedEngine()->m_system->getChannel(126 + i, &audioChannel);
                if (audioChannel) {
                    uint32_t channelTime = 0;
                    audioChannel->getPosition(&channelTime, FMOD_TIMEUNIT_MS);

                    if (channelTime <= 0)
                        continue;

                    if (channelTime - songTime > 0.25f || channelTime - songTime < -0.25f)
                        audioChannel->setPosition(songTime, FMOD_TIMEUNIT_MS);
                }
            }
        }

        void update(float dt) {
            if ((!s_recorder.isRecording() && !s_recorder.isRecordingAudio()) || m_gameState.m_currentProgress <= 0) return GJBaseGameLayer::update(dt);

            float endscreen = config::get<float>("recorder.endscreen", 5.f);

            if (levelDone) {
                if (afterEndTimer > endscreen) {
                    if (s_recorder.isRecording() && !popupShown) {
                        switch(config::get<int>("recorder.audio", 2)) {
                            case 1:
                                popupShown = true;
                                Popup::create(
                                    i18n::get_("recorder.audio"),
                                    i18n::get_("recorder.audio.msg"),
                                    i18n::get_("common.yes"),
                                    i18n::get_("common.no"),
                                    [&](bool result) {
                                        if(result) {
                                            startAudio();
                                            return;
                                        }
                                        stop();
                                        endPopup();
                                    }
                                );
                                break;
                            case 2:
                                startAudio();
                                break;
                            default:
                            case 0:
                                stop();
                                endPopup();
                                break;
                        }
                    }
                    else if (s_recorder.isRecordingAudio()) {
                        stopAudio();
                        endPopup();
                    }
                    
                    return GJBaseGameLayer::update(dt);
                }
                afterEndTimer += dt;
            }

            if (!s_recorder.isRecording())
                return GJBaseGameLayer::update(dt);

            float fps = config::get<float>("recorder.fps", 60.f);
            float timewarp = m_gameState.m_timeWarp;

            totalTime += dt;

            double frameDt = 1. / static_cast<double>(fps) * timewarp;
            double time = totalTime + extraTime - lastFrameTime;

            if (time >= frameDt) {
                extraTime = time - frameDt;
                lastFrameTime = totalTime;

                syncMusic();

                visiting = true;
                s_recorder.captureFrame();
                visiting = false;
            }

            GJBaseGameLayer::update(dt);
        }
    };
    
    class $modify(InternalRecorderPLHook, PlayLayer) {
        void onQuit() {
            if (s_recorder.isRecording()) stop();
            if (s_recorder.isRecordingAudio()) stopAudio();
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

        void pauseGame(bool paused) {
            if (s_recorder.isRecordingAudio())
                return;

            PlayLayer::pauseGame(paused);
        }
    };

};