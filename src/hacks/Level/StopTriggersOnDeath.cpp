#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(StopTriggersOnDeath) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.stoptrigondeath")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() const override {
            if (config::get<"level.stoptrigondeath", bool>(false)) {
                if (auto pl = utils::get<PlayLayer>()) {
                    if (pl->m_level->isPlatformer()) return true;
                }
            }
            return false;
        }

        [[nodiscard]] const char* getId() const override { return "Stop Triggers On Death"; }
    };

    REGISTER_HACK(StopTriggersOnDeath)

    class $modify(StopTriggersOnDeathGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("level.stoptrigondeath")

        void update(float dt) {
            if (!utils::get<PlayLayer>()) return GJBaseGameLayer::update(dt);

            if ((m_player1 && m_player1->m_isDead) || (m_player2 && m_player2->m_isDead)) return;

            GJBaseGameLayer::update(dt);
        }
    };
}
