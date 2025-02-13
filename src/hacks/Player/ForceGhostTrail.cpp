#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $modify(PlayerObjectFGTHook, PlayerObject){
        struct Fields {
            GhostType m_curGhostType = GhostType::Disabled;
        };

        ADD_HOOKS_DELEGATE("player.forceghosttrail")

        void toggleGhostEffect(GhostType p0) {
            m_fields->m_curGhostType = p0;
            if (PlayerObject::m_isDead != true) p0 = GhostType::Enabled;
            PlayerObject::toggleGhostEffect(p0);
        }
    };

    class $hack(ForceGhostTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.forceghosttrail")->setDescription()->handleKeybinds()
            ->callback([] {
                auto* gjbgl = utils::get<GJBaseGameLayer>();
                if (!gjbgl) return;

                auto p1 = static_cast<PlayerObjectFGTHook*>(gjbgl->m_player1);
                auto p2 = static_cast<PlayerObjectFGTHook*>(gjbgl->m_player2);
                p1->toggleGhostEffect(p1->m_fields->m_curGhostType);
                if (gjbgl->m_gameState.m_isDualMode) p2->toggleGhostEffect(p2->m_fields->m_curGhostType);
            });
        }
      
        [[nodiscard]] const char* getId() const override { return "Force Ghost Trail"; }
    };

    REGISTER_HACK(ForceGhostTrail)
}
