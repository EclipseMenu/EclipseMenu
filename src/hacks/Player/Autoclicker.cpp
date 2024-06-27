#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class AutoClicker : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("AutoClicker", "player.autoclick");
        }

        [[nodiscard]] const char* getId() const override { return "AutoClicker"; }
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
                m_fields->thedt += dt;
                if (m_fields->thedt > 0.1f) { // FIXME: doesn't click
                    m_fields->clicking = !m_fields->clicking;
                    m_fields->clicking ? m_player1->releaseButton(PlayerButton::Jump) : m_player1->pushButton(PlayerButton::Jump);
                    m_fields->thedt = 0.f;
                }
            }
        }
    };

}
