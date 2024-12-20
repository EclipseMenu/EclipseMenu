#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class StopTriggersOnDeath : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.stoptrigondeath")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Stop Triggers On Death"; }
    };

    REGISTER_HACK(StopTriggersOnDeath)

    class $modify(StopTriggersOnDeathGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("level.stoptrigondeath")

        void update(float dt) {
            if (!PlayLayer::get()) return GJBaseGameLayer::update(dt);

            if ((m_player1 && m_player1->m_isDead) || (m_player2 && m_player2->m_isDead)) return;

            GJBaseGameLayer::update(dt);
        }
    };
}