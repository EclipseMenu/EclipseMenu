#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class NoDeathEffect : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.nodeatheffect")
                ->setDescription()
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Death Effect"; }
    };

    REGISTER_HACK(NoDeathEffect)

    class $modify(NoDeathEffectPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.nodeatheffect")
        void playDeathEffect() {}
    };

}
