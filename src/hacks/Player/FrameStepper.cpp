#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCScheduler.hpp>

namespace eclipse::hacks::Player {

    class FrameStepper : public hack::Hack {
        void init() override {
            config::setIfEmpty("player.framestepper", false);
            config::setIfEmpty("player.framestepper.step_key", keybinds::Keys::C);
            config::setIfEmpty("player.framestepper.hold", true);
            config::setIfEmpty("player.framestepper.hold_delay", 0.25f);
            config::setIfEmpty("player.framestepper.hold_speed", 5);

            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("Frame Stepper", "player.framestepper")
                ->setDescription("Allows you to step through the game frame by frame.")
                ->addOptions([] (auto* options) {
                    options->addKeybind("Step Key", "player.framestepper.step_key");
                    options->addToggle("Enable Hold", "player.framestepper.hold");
                    options->addInputFloat("Hold Delay", "player.framestepper.hold_delay", 0.0f, FLT_MAX, "%.2f");
                    options->addInputInt("Hold Speed", "player.framestepper.hold_speed", 0);
                });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.framestepper", false); }
        [[nodiscard]] const char* getId() const override { return "Frame Stepper"; }
    };

    REGISTER_HACK(FrameStepper)

    static double s_holdDelayTimer = 0;
    static uint32_t s_holdAdvanceTimer = 0;

    class $modify(cocos2d::CCScheduler) {
        static void onModify(auto& self) {
            FIRST_PRIORITY("cocos2d::CCScheduler::update"); // required to avoid conflict with speedhack
        }

        void update(float dt) override {
            if (!config::get<bool>("player.framestepper", false))
                return cocos2d::CCScheduler::update(dt);

            // Make it only work on GJBGL
            if (!GJBaseGameLayer::get())
                return cocos2d::CCScheduler::update(dt);

            // for playlayer, check if the level is not paused/finished (maybe add loading check later?)
            bool usable = false;
            if (auto playLayer = PlayLayer::get()) {
                usable = !playLayer->m_isPaused && !playLayer->m_hasCompletedLevel;
            }
            // for level editor, check if it's in playback mode
            else if (auto editor = LevelEditorLayer::get()) {
                usable = editor->m_playbackMode == PlaybackMode::Playing;
            }

            if (!usable) return cocos2d::CCScheduler::update(dt);

            auto step = 240.f; // TODO: Change this after Physics Bypass is added

            auto stepKey = config::get<keybinds::Keys>("player.framestepper.step_key", keybinds::Keys::C);
            auto holdSpeed = config::get<int>("player.framestepper.hold_speed", 5);
            bool shouldStep = false;
            if (config::get<bool>("player.framestepper.hold", false)) {
                s_holdDelayTimer += dt;
                bool firstPress = keybinds::isKeyPressed(stepKey);
                if (firstPress) {
                    s_holdDelayTimer = 0;
                }

                // Add a grace period after the first press to allow for holding
                shouldStep = keybinds::isKeyDown(stepKey);
                auto delay = config::get<float>("player.framestepper.hold_delay", 0.25f);
                if (shouldStep) {
                    s_holdAdvanceTimer++;
                    if (s_holdAdvanceTimer >= holdSpeed) {
                        s_holdAdvanceTimer = 0;
                        shouldStep = s_holdDelayTimer >= delay;
                    }
                    shouldStep |= firstPress;
                } else {
                    s_holdAdvanceTimer = 0;
                }
            } else {
                shouldStep = keybinds::isKeyPressed(stepKey);
            }

            dt = shouldStep ? 1.f / step : 0.f;
            cocos2d::CCScheduler::update(dt);
        }
    };

}
