#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class JumpHack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Jump Hack", "player.jumphack")
                ->setDescription("Allows the player to infinitely jump in the air")
                ->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.jumphack", false); }
        [[nodiscard]] const char* getId() const override { return "Jump Hack"; }
    };

    REGISTER_HACK(JumpHack)

    bool jump = false;

    class $modify(JumpHackBGLHook, GJBaseGameLayer) {
        void update(float dt) {
            if (config::get<bool>("player.jumphack", false) && jump) {
                if (m_player1) m_player1->m_isOnGround = true;
                if (m_player2) m_player2->m_isOnGround = true;
            }

            GJBaseGameLayer::update(dt);

            if (config::get<bool>("player.jumphack", false) && jump) {
                if (m_player1) m_player1->m_isOnGround = true;
                if (m_player2) m_player2->m_isOnGround = true;
                jump = false;
            }
        }
    };

    class $modify(JumpHackPOHook, PlayerObject) {
        bool pushButton(PlayerButton p0) {
            jump = true;
            return PlayerObject::pushButton(p0);
        }
    };
}
