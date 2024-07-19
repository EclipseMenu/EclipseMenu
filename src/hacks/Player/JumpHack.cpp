#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class JumpHack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("Jump Hack", "player.jumphack")->setDescription("Allows the player to infinitely jump in the air")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Jump Hack"; }
    };

    REGISTER_HACK(JumpHack)

    bool jump = false;

    class $modify(GJBaseGameLayer) {
        void update(float dt) {
            if (config::get<bool>("player.jumphack") && jump) {
                if (m_player1) m_player1->m_isOnGround = true;
                if (m_player2) m_player2->m_isOnGround = true;
            }
            GJBaseGameLayer::update(dt);
            if (config::get<bool>("player.jumphack") && jump) {
                if (m_player1) m_player1->m_isOnGround = true;
                if (m_player2) m_player2->m_isOnGround = true;
                jump = false;
            }
        }
    };

    class $modify(PlayerObject) {
        void pushButton(PlayerButton idk) {
            jump = true;
            PlayerObject::pushButton(idk);
        }
    };
}
