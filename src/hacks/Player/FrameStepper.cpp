#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/UILayer.hpp>

namespace eclipse::hacks::Player {

    // for on-screen UI
    static bool s_frameStepperPressed = false;
    static bool s_frameStepperDown = false;

    class FrameStepper : public hack::Hack {
    public:
        static bool isPressed() {
            auto stepKey = config::get<keybinds::Keys>("player.framestepper.step_key", keybinds::Keys::C);
            return s_frameStepperPressed || keybinds::isKeyPressed(stepKey);
        }

        static bool isDown() {
            auto stepKey = config::get<keybinds::Keys>("player.framestepper.step_key", keybinds::Keys::C);
            return s_frameStepperDown || keybinds::isKeyDown(stepKey);
        }

    private:
        void init() override {
            config::setIfEmpty("player.framestepper", false);
            config::setIfEmpty("player.framestepper.step_key", keybinds::Keys::C);
            config::setIfEmpty("player.framestepper.hold", true);
            config::setIfEmpty("player.framestepper.hold_delay", 0.25f);
            config::setIfEmpty("player.framestepper.hold_speed", 5);

            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Frame Stepper", "player.framestepper")
                ->setDescription("Allows you to step through the game frame by frame.")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
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
                usable = !playLayer->m_isPaused && !playLayer->m_hasCompletedLevel; // && playLayer->m_started;
            }
            // for level editor, check if it's in playback mode
            else if (auto editor = LevelEditorLayer::get()) {
                usable = editor->m_playbackMode == PlaybackMode::Playing;
            }

            if (!usable) return cocos2d::CCScheduler::update(dt);

            auto step = 240.f; // TODO: Change this after Physics Bypass is added

            auto holdSpeed = config::get<int>("player.framestepper.hold_speed", 5);
            bool shouldStep = false;
            if (config::get<bool>("player.framestepper.hold", false)) {
                s_holdDelayTimer += dt;
                bool firstPress = FrameStepper::isPressed();
                if (firstPress) {
                    s_holdDelayTimer = 0;
                }

                // Add a grace period after the first press to allow for holding
                auto delay = config::get<float>("player.framestepper.hold_delay", 0.25f);
                if (FrameStepper::isDown()) {
                    shouldStep = firstPress;
                    s_holdAdvanceTimer++;
                    if (!shouldStep && s_holdAdvanceTimer >= holdSpeed) {
                        s_holdAdvanceTimer = 0;
                        shouldStep = s_holdDelayTimer >= delay;
                    }
                } else {
                    s_holdAdvanceTimer = 0;
                }
            } else {
                shouldStep = FrameStepper::isPressed();
            }

            s_frameStepperPressed = false;
            dt = shouldStep ? 1.f / step : 0.f;
            cocos2d::CCScheduler::update(dt);
        }
    };

    /// @brief A button with callbacks for holding and releasing
    class HoldingMenuItem : public CCMenuItemSpriteExtra {
    public:
        static HoldingMenuItem* create(cocos2d::CCSprite* sprite, const std::function<void()>& onHold, const std::function<void()>& onRelease) {
            auto ret = new HoldingMenuItem();
            if (ret->init(sprite, onHold, onRelease)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        bool init(cocos2d::CCSprite* sprite, const std::function<void()>& onHold, const std::function<void()>& onRelease) {
            if (!CCMenuItemSpriteExtra::init(sprite, nullptr, nullptr, nullptr))
                return false;

            m_onHold = onHold;
            m_onRelease = onRelease;

            return true;
        }

        void selected() override {
            CCMenuItemSpriteExtra::selected();
            m_onHold();
        }

        void unselected() override {
            CCMenuItemSpriteExtra::unselected();
            m_onRelease();
        }
    private:
        std::function<void()> m_onHold;
        std::function<void()> m_onRelease;
    };

    class FrameStepControl : public cocos2d::CCMenu {
        bool init() override {
            if (!cocos2d::CCMenu::init())
                return false;

            this->setPosition(0, 0);

            auto winSize = cocos2d::CCDirector::get()->getWinSize();
            auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            sprite->setFlipX(true);
            m_stepForward = HoldingMenuItem::create(sprite,
                [] { 
                    s_frameStepperPressed = true;
                    s_frameStepperDown = true;
                },
                [] { s_frameStepperDown = false; }
            );
            m_stepForward->setPosition(winSize.width - 50, winSize.height / 2);
            this->addChild(m_stepForward);

            this->schedule(schedule_selector(FrameStepControl::update), 0.f);

            return true;
        }

        void update(float dt) override {
            this->setVisible(config::get<bool>("player.framestepper", false));
        }

    public:
        static FrameStepControl* create() {
            auto ret = new FrameStepControl();
            if (ret->init()) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    private:
        HoldingMenuItem* m_stepForward = nullptr;
    };

    // Desktop users don't need the on-screen UI
#ifndef GEODE_IS_DESKTOP
    class $modify(UILayer) {
        bool init(GJBaseGameLayer* bgl) {
            if (!UILayer::init(bgl))
                return false;

            auto btn = FrameStepControl::create();
            btn->setID("frame-stepper"_spr);
            this->addChild(btn, 1000);
            return true;
        }
    };
#endif

}
