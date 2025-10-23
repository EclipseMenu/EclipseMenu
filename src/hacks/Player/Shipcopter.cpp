#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $hack(Shipcopter) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.shipcopter")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"player.shipcopter", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Shipcopter"; }
    };

    REGISTER_HACK(Shipcopter)

    class $modify(SCPlayerObjectHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.shipcopter")

        bool pushButton(PlayerButton p0) {
            if (!m_gameLayer) return PlayerObject::pushButton(p0);

            bool ret = PlayerObject::pushButton(p0);
            if (ret && m_isSwing) {
                this->flipGravity(m_isUpsideDown, true);
            }

            return ret;
        }

        bool releaseButton(PlayerButton p0) {
            if (!m_gameLayer) return PlayerObject::releaseButton(p0);

            bool ret = PlayerObject::releaseButton(p0);
            if (ret && m_isSwing) {
                this->flipGravity(!m_isUpsideDown, true);
            }

            return ret;
        }
    };
}