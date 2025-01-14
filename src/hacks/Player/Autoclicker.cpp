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
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addToggle("player.autoclick.p1");
                   options->addToggle("player.autoclick.p2");
                   options->addInputInt("player.autoclick.intervalhold", 1, 1000);
                   options->addInputInt("player.autoclick.intervalrelease", 1, 1000);
               });
        }

        [[nodiscard]] bool isCheating() const override { RETURN_CACHED_BOOL("player.autoclick"); }
        [[nodiscard]] const char* getId() const override { return "Auto Clicker"; }
    };

    REGISTER_HACK(AutoClicker)

    class $modify(AutoClickerBGLHook, GJBaseGameLayer) {
        struct Fields {
            int timer = 0;
            bool clicking = false;
        };

        ADD_HOOKS_DELEGATE("player.autoclick")

        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            auto clickInterval = config::get<int>("player.autoclick.intervalhold", 1);
            auto releaseInterval = config::get<int>("player.autoclick.intervalrelease", 1);
            m_fields->timer++;
            if ((m_fields->timer >= clickInterval && !m_fields->clicking) || (m_fields->timer >= releaseInterval && m_fields->clicking)) {
                m_fields->clicking = !m_fields->clicking;
                if (config::get<bool>("player.autoclick.p1")) this->handleButton(m_fields->clicking, 1, true);
                if (config::get<bool>("player.autoclick.p2")) this->handleButton(m_fields->clicking, 1, false);
                m_fields->timer = 0.f;
            }
        }
    };
}
