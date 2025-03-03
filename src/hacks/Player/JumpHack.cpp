#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $hack(JumpHack) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.jumphack")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"player.jumphack", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Jump Hack"; }
    };

    REGISTER_HACK(JumpHack)

    bool jump = false;

    class $modify(JumpHackBGLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("player.jumphack")

        void update(float dt) {
            if (jump) {
                if (m_player1) m_player1->m_isOnGround = true;
                if (m_player2) m_player2->m_isOnGround = true;
            }

            GJBaseGameLayer::update(dt);

            if (jump) {
                if (m_player1) m_player1->m_isOnGround = true;
                if (m_player2) m_player2->m_isOnGround = true;
                jump = false;
            }
        }
    };

    class $modify(JumpHackPOHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.jumphack")

        bool pushButton(PlayerButton p0) {
            jump = true;
            return PlayerObject::pushButton(p0);
        }
    };
}
