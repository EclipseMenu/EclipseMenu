#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class AutoClicker : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty("player.autoclick", false);

            gui::ToggleComponent* toggle = tab->addToggle("AutoClicker", "player.autoclick")->handleKeybinds();

            config::setIfEmpty<float>("player.autoclick.interval", 1.f);
            
            toggle->addOptions([](gui::MenuTab* options) {
                options->addInputFloat("Interval", "player.autoclick.interval", 0.f, 10.f, "%.3f s.");
            });

        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.autoclick", false); }
        [[nodiscard]] const char* getId() const override { return "Auto Clicker"; }
    };

    REGISTER_HACK(AutoClicker)

    class $modify(GJBaseGameLayer) {
        struct Fields {
            float thedt = 0.f;
            bool clicking = false;
        };

        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (config::get<bool>("player.autoclick", false)) {
                float clickInterval = config::get<float>("player.autoclick.interval", 0.f);
                m_fields->thedt += dt;
                if (m_fields->thedt > clickInterval) { // FIXME: doesn't click
                    m_fields->clicking = !m_fields->clicking;
                    m_fields->clicking ? m_player1->releaseButton(PlayerButton::Jump) : m_player1->pushButton(PlayerButton::Jump);
                    m_fields->thedt = 0.f;
                }
            }
        }
    };

}
