#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class NoPlatformerJumpSquish : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.noplatformersquish")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Platformer Jump Anim"; }
    };

    REGISTER_HACK(NoPlatformerJumpSquish)

    class $modify(NoPlatformerJumpSquishPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.noplatformersquish")
        void animatePlatformerJump(float) {}
    };
}
