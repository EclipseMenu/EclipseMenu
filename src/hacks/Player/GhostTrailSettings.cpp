#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $modify(PlayerObjectGTHook, PlayerObject){
        struct Fields {
            GhostType m_curGhostType = GhostType::Disabled;
        };

        void toggleGhostEffect(GhostType p0) {
            m_fields->m_curGhostType = p0;
            PlayerObject::toggleGhostEffect(p0);
        }
    };

    class $hack(ForceGhostTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.forceghosttrail")->setDescription()->handleKeybinds()
            ->callback([](bool v) {
                config::set("player.noghosttrail", false);
                auto* pl = utils::get<PlayLayer>();
                if (!pl) return;

                auto p1 = static_cast<PlayerObjectGTHook*>(pl->m_player1);
                auto p2 = static_cast<PlayerObjectGTHook*>(pl->m_player2);
                p1->toggleGhostEffect(p1->m_fields->m_curGhostType);
                if (pl->m_gameState.m_isDualMode) p2->toggleGhostEffect(p2->m_fields->m_curGhostType);
            });
        }

        [[nodiscard]] const char* getId() const override { return "Force Ghost Trail"; }
    };

    class $hack(NoGhostTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.noghosttrail")->setDescription()->handleKeybinds()
            ->callback([](bool v) {
                config::set("player.forceghosttrail", false);
                auto* pl = utils::get<PlayLayer>();
                if (!pl) return;

                auto p1 = static_cast<PlayerObjectGTHook*>(pl->m_player1);
                auto p2 = static_cast<PlayerObjectGTHook*>(pl->m_player2);
                p1->toggleGhostEffect(p1->m_fields->m_curGhostType);
                if (pl->m_gameState.m_isDualMode) p2->toggleGhostEffect(p2->m_fields->m_curGhostType);
            });
        }

        [[nodiscard]] const char* getId() const override { return "No Ghost Trail"; }
    };

    REGISTER_HACK(ForceGhostTrail)
    REGISTER_HACK(NoGhostTrail)

    class $modify(PlayerObjectFGTHookImpl, PlayerObject){
        ADD_HOOKS_DELEGATE("player.forceghosttrail");

        void toggleGhostEffect(GhostType p0) {
            if (!this->m_isDead && utils::get<PlayLayer>()) p0 = GhostType::Enabled;
            PlayerObject::toggleGhostEffect(p0);
        }
    };

    class $modify(PlayerObjectNGTHookImpl, PlayerObject){
        ADD_HOOKS_DELEGATE("player.noghosttrail");

        void toggleGhostEffect(GhostType p0) {
            p0 = GhostType::Disabled;
            PlayerObject::toggleGhostEffect(p0);
        }
    };
}
