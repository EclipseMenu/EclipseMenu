#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/recorder/recorder.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/ShaderLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

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

    intptr_t glViewportAddress = 0;

    void endPopup() {
        Popup::create("Info", "Recording finished!", "OK", "Open folder", [](bool result) {
            if(result)
                return;

            geode::utils::file::openFolder(geode::Mod::get()->getSaveDir() / "renders");
        });
    }
    
    void glViewportHook(GLint a, GLint b, GLsizei c, GLsizei d) {
        if (visiting && s_recorder.isRecording() && inShaderLayer) {
            ImVec2 displaySize = ImGui::GetIO().DisplaySize;
            //shaderlayer resolutions for each quality mode
            if (c != 2608 && d != 2608 && c != 1304 && d != 1304 && c != 652 && d != 652 && c == (int)displaySize.x && d == (int)displaySize.y) {
                c = config::get<int>("recorder.resolution.x", 1920);
                d = config::get<int>("recorder.resolution.y", 1080);
            }
        }

        reinterpret_cast<void(__stdcall *)(GLint, GLint, GLsizei, GLsizei)>(glViewportAddress)(a, b, c, d);
    }

    $execute {
        glViewportAddress = geode::addresser::getNonVirtual(glViewport);
        auto result = geode::Mod::get()->hook(reinterpret_cast<void *>(glViewportAddress), &glViewportHook, "glViewport");
        if (result.isErr())
            geode::log::error("Failed to hook glViewport");
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

        auto lvl = PlayLayer::get()->m_level;

        std::filesystem::path renderDirectory = geode::Mod::get()->getSaveDir() / "renders" / lvl->m_levelName;

        if (!std::filesystem::exists(renderDirectory))
            std::filesystem::create_directories(renderDirectory);

        s_recorder.m_renderSettings.m_bitrate = static_cast<int>(config::get<float>("recorder.bitrate", 30.f)) * 1000000;
        s_recorder.m_renderSettings.m_fps = static_cast<int>(config::get<float>("recorder.fps", 60.f));
        s_recorder.m_renderSettings.m_width = config::get<int>("recorder.resolution.x", 1920);
        s_recorder.m_renderSettings.m_height = config::get<int>("recorder.resolution.y", 1080);
        s_recorder.m_renderSettings.m_codecId = config::get<int>("recorder.codecId", 27);
        s_recorder.m_renderSettings.m_outputFile = renderDirectory / (fmt::format("{} - {}.mp4", lvl->m_levelName, lvl->m_levelID.value()));
        s_recorder.m_renderSettings.m_hardwareAccelerationType = static_cast<ffmpeg::HardwareAccelerationType>(config::get<int>("recorder.hwType", 0));

        s_recorder.start();
    }

    void stop() {
        s_recorder.stop();
    }

    void startAudio() {
        stop();

        auto lvl = PlayLayer::get()->m_level;
        auto renderPath = geode::Mod::get()->getSaveDir() / "renders" / STR(lvl->m_levelName) / (fmt::format("{} - {}.mp4", lvl->m_levelName, lvl->m_levelID.value()));

        if (!std::filesystem::exists(renderPath)) {
            geode::log::error("Render {} not found", renderPath);
            return;
        }

        levelDone = false;
        popupShown = false;
        afterEndTimer = 0.f;

        if (PlayLayer::get()->getChildByID("EndLevelLayer"))
            PlayLayer::get()->getChildByID("EndLevelLayer")->removeFromParent();

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
            auto tab = gui::MenuTab::find("Internal Recorder");

            tab->addButton("Start Recording")->callback(start);
            tab->addButton("Stop Recording")->callback([] {
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

            m_codecs = s_recorder.getAvailableCodecs();

            for(auto& codec : m_codecs)
                m_codecNames.push_back(codec.first);

            int h264Id = m_codecs.at("h264");

            tab->addInputFloat("Framerate", "recorder.fps", 1.f, 360.f, "%.0f FPS");
            tab->addInputFloat("Endscreen Duration", "recorder.endscreen", 0.f, 30.f, "%.2fs.");
            tab->addInputFloat("Bitrate", "recorder.bitrate", 1.f, 1000.f, "%.0fmbps");
            tab->addInputInt("Resolution X", "recorder.resolution.x", 1, 15360);
            tab->addInputInt("Resolution Y", "recorder.resolution.y", 1, 8640);

            config::setIfEmpty("recorder.codecId", h264Id);

            tab->addCombo("Codec", "recorder.codecIdx", m_codecNames, static_cast<int>(std::distance(m_codecNames.begin(), std::find(m_codecNames.begin(), m_codecNames.end(), "h264"))))->callback([&](int index) {
                config::set("recorder.codecId", m_codecs[m_codecNames[index]]);
            });

            tab->addCombo("HW Type", "recorder.hwIdx", {"None", "CUDA (Nvidia)", "D3D11 (All)"}, 0)->callback([&](int index) {
                switch(index) {
                    case 0:
                        config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::NONE));
                        break;
                    case 1:
                        config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::CUDA));
                        break;
                    case 2:
                        config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::D3D11VA));
                        break;
                }
            });

            tab->addCombo("Audio mode", "recorder.audio", {"Don't record", "Ask first", "Always record"}, 0);
            
            tab->addLabel("Presets");
            tab->addButton("CPU")->callback([] {
                config::set<std::string>("recorder.codecString", "h264");
            });

            tab->addButton("NVIDIA")->callback([] {
                config::set<std::string>("recorder.codecString", "h264_nvenc");
                config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::CUDA));
            });

            tab->addButton("AMD")->callback([] {
                config::set<std::string>("recorder.codecString", "h264_amf");
                config::set<int>("recorder.hwType", static_cast<int>(ffmpeg::HardwareAccelerationType::D3D11VA));
            });
        }

        [[nodiscard]] const char* getId() const override { return "Internal Recorder"; }

        std::vector<std::string> m_codecNames;
        std::unordered_map<std::string, int> m_codecs;
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
                float framerate = 60.f;

                framerate = GameManager::get()->m_customFPSTarget;

                if (framerate < 1)
                    framerate = 1;

                dt = 1.f / framerate;
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

                    if (channelTime - songTime > 0.05f)
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
                                Popup::create("Audio", "Record audio?", "Yes", "No", [&](bool result) {
                                    if(result) {
                                        startAudio();
                                        return;
                                    }
                                    stop();
                                    endPopup();
                                });
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
                else
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