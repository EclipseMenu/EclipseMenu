#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class AutoClicker : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty<bool>("player.autoclick.p1", true);
            config::setIfEmpty<bool>("player.autoclick.p2", true);
            config::setIfEmpty<int>("player.autoclick.intervalrelease", 1);

            tab->addToggle("AutoClicker", "player.autoclick")
                ->handleKeybinds()
                ->setDescription("Clicks periodically when playing levels. Applies to both the level editor and actual levels.")
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addToggle("Player 1", "player.autoclick.p1");
                    options->addToggle("Player 2", "player.autoclick.p2");
                    options->addInputInt("Hold Interval", "player.autoclick.intervalhold", 1, 1000);
                    options->addInputInt("Release Interval", "player.autoclick.intervalrelease", 1, 1000);
                });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.autoclick", false); }
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
