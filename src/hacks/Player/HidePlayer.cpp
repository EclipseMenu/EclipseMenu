#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $modify(PlayerObjectHPHook, PlayerObject){
        struct Fields {
            bool m_isPlayerInvis = false;
        };

        void toggleVisibility(bool p0) {
            m_fields->m_isPlayerInvis = p0;
            PlayerObject::toggleVisibility(p0);
        }
    };

    class $hack(HidePlayer) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.hideplayer")->setDescription()->handleKeybinds()
            ->callback([](bool v) {
                auto* gjbgl = utils::get<GJBaseGameLayer>();
                if (!gjbgl) return;

                auto p1 = static_cast<PlayerObjectHPHook*>(gjbgl->m_player1);
                auto p2 = static_cast<PlayerObjectHPHook*>(gjbgl->m_player2);
                p1->toggleVisibility(p1->m_fields->m_isPlayerInvis);
                if (gjbgl->m_gameState.m_isDualMode) p2->toggleVisibility(p2->m_fields->m_isPlayerInvis);
            });
        }
      
        [[nodiscard]] const char* getId() const override { return "Hide Player"; }
    };

    REGISTER_HACK(HidePlayer)
    
    class $modify(PlayerObjectHPHookImpl, PlayerObject){
        ADD_HOOKS_DELEGATE("player.hideplayer")

        void toggleVisibility(bool p0) {
            PlayerObject::toggleVisibility(false);
        }
    };
}
