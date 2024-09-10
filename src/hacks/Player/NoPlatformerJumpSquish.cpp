#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class NoPlatformerJumpSquish : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Platformer Jump Anim", "player.noplatformersquish")
                ->setDescription("Disables the squishing animation when jumping in place inside platformer levels. Applies to both the level editor and actual levels. (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Platformer Jump Anim"; }
    };

    REGISTER_HACK(NoPlatformerJumpSquish)

    class $modify(NoPlatformerJumpSquishPOHook, PlayerObject) {
        void animatePlatformerJump(float p0) {
            if (!config::get<bool>("player.noplatformersquish", false))
                PlayerObject::animatePlatformerJump(p0);
        }
    };

}