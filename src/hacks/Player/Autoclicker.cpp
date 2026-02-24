#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {
    class $hack(AutoClicker) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");

            config::setIfEmpty<bool>("player.autoclick.p1", true);
            config::setIfEmpty<bool>("player.autoclick.p2", true);
            config::setIfEmpty<int>("player.autoclick.intervalrelease", 1);

            tab->addToggle("player.autoclick")
               ->handleKeybinds()
               ->setDescription()
               ->addOptions([](auto options) {
                   options->addToggle("player.autoclick.p1");
                   options->addToggle("player.autoclick.p2");
                   options->addInputInt("player.autoclick.intervalhold", 1, 1000);
                   options->addInputInt("player.autoclick.intervalrelease", 1, 1000);
               });
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"player.autoclick", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Auto Clicker"; }
    };

    REGISTER_HACK(AutoClicker)

    class $modify(AutoClickerBGLHook, GJBaseGameLayer) {
        struct Fields {
            int timer = 0;
            bool clicking = false;
        };

        ADD_HOOKS_DELEGATE("player.autoclick")

        void updateAutoClicker() {
            auto clickInterval = config::get<"player.autoclick.intervalhold", int>(1);
            auto releaseInterval = config::get<"player.autoclick.intervalrelease", int>(1);
            auto fields = m_fields.self();

            fields->timer++;
            if ((fields->timer >= clickInterval && !fields->clicking) || (fields->timer >= releaseInterval && fields->clicking)) {
                fields->clicking = !fields->clicking;
                if (config::get<"player.autoclick.p1", bool>(false)) this->handleButton(fields->clicking, 1, true);
                if (config::get<"player.autoclick.p2", bool>(false)) this->handleButton(fields->clicking, 1, false);
                fields->timer = 0.f;
            }
        }

        #ifndef GEODE_IS_MACOS
        void processCommands(float dt, bool isHalfTick, bool isLastTick) {
            GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
            this->updateAutoClicker();
        }
        #else
        void processQueuedButtons(float dt, bool clearInputQueue) {
            GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
            this->updateAutoClicker();
        }
        #endif
    };
}
