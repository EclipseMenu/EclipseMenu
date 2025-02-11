#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/keybind.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/UILayer.hpp>

namespace eclipse::hacks::Player {
    // for on-screen UI
    static bool s_frameStepperPressed = false;
    static bool s_frameStepperDown = false;

    class $hack(FrameStepper) {
        static bool isPressed() {
            auto stepKey = config::get<"player.framestepper.step_key", keybinds::Keys>(keybinds::Keys::C);
            return s_frameStepperPressed || keybinds::isKeyPressed(stepKey);
        }

        static bool isDown() {
            auto stepKey = config::get<"player.framestepper.step_key", keybinds::Keys>(keybinds::Keys::C);
            return s_frameStepperDown || keybinds::isKeyDown(stepKey);
        }

        void init() override {
            config::setIfEmpty("player.framestepper", false);
            config::setIfEmpty("player.framestepper.step_key", keybinds::Keys::C);
            config::setIfEmpty("player.framestepper.hold", true);
            config::setIfEmpty("player.framestepper.hold_delay", 0.25f);
            config::setIfEmpty("player.framestepper.hold_speed", 5);

            auto tab = gui::MenuTab::find("tab.player");

            tab->addToggle("player.framestepper")
               ->setDescription()
               ->handleKeybinds()
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addKeybind("player.framestepper.step_key", "player.framestepper.step_key")
                          ->setDefaultKey(keybinds::Keys::C);
                   options->addToggle("player.framestepper.hold");
                   options->addInputFloat("player.framestepper.hold_delay", 0.0f, FLT_MAX, "%.2f");
                   options->addInputInt("player.framestepper.hold_speed", 0);
               });
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"player.framestepper", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Frame Stepper"; }
    };

    REGISTER_HACK(FrameStepper)

    static double s_holdDelayTimer = 0;
    static uint32_t s_holdAdvanceTimer = 0;

    class $modify(FrameStepperSchedulerHook, GJBaseGameLayer) {
        static void onModify(auto& self) {
            FIRST_PRIORITY("cocos2d::CCScheduler::update"); // required to avoid conflict with speedhack
            HOOKS_TOGGLE_ALL("player.framestepper");
        }

        void update(float dt) override {
            // for playlayer, check if the level is not paused/finished (maybe add loading check later?)
            bool usable = false;
            if (auto playLayer = utils::get<PlayLayer>())
                usable = !playLayer->m_isPaused && !playLayer->m_hasCompletedLevel && playLayer->m_started && !playLayer->m_player1->m_isDead;

            // for level editor, check if it's in playback mode
            else if (auto editor = utils::get<LevelEditorLayer>())
                usable = editor->m_playbackMode == PlaybackMode::Playing;

            if (!usable)
                return GJBaseGameLayer::update(dt);

            auto step = utils::getTPS();

            auto holdSpeed = config::get<int>("player.framestepper.hold_speed", 5);
            bool shouldStep = false;
            if (config::get<bool>("player.framestepper.hold", false)) {
                s_holdDelayTimer += dt;
                bool firstPress = FrameStepper::isPressed();

                if (firstPress)
                    s_holdDelayTimer = 0;

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
            GJBaseGameLayer::update(dt);
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
            if (!CCMenu::init())
                return false;

            this->setPosition(0, 0);
            this->setVisible(config::get<"player.framestepper", bool>());

            auto winSize = utils::get<cocos2d::CCDirector>()->getWinSize();
            auto sprite = cocos2d::CCSprite::createWithSpriteFrameName("GJ_arrow_01_001.png");
            sprite->setFlipX(true);
            m_stepForward = HoldingMenuItem::create(
                sprite, [] {
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
            this->setVisible(config::get<"player.framestepper", bool>());
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
    class $modify(FrameSFrameStepperUILHook, UILayer) {
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
