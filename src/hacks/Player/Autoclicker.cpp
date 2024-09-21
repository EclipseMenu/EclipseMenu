#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class AutoClicker : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty<float>("player.autoclick.interval", 1.f);

            tab->addToggle("AutoClicker", "player.autoclick")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addInputFloat("Interval", "player.autoclick.interval", 0.f, 10.f, "%.3f s.");
                });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.autoclick", false); }
        [[nodiscard]] const char* getId() const override { return "Auto Clicker"; }
    };

    REGISTER_HACK(AutoClicker)

    class $modify(AutoClickerBGLHook, GJBaseGameLayer) {
        struct Fields {
            float timer = 0.f;
            bool clicking = false;
        };

        ADD_HOOKS_DELEGATE("player.autoclick")

        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            auto clickInterval = config::get<float>("player.autoclick.interval", 0.f);
            m_fields->timer += dt;
            if (m_fields->timer > clickInterval) {
                m_fields->clicking = !m_fields->clicking;
                this->handleButton(m_fields->clicking, 1, true);
                m_fields->timer = 0.f;
            }
        }
    };

}
