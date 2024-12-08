#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class NoGhostTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");

            tab->addToggle("player.noghosttrail")
                ->setDescription()
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Ghost Trail"; }
    };

    REGISTER_HACK(NoGhostTrail)

    class $modify(NoGhostTrailPOHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.noghosttrail")

        void toggleGhostEffect(GhostType) {
            PlayerObject::toggleGhostEffect(GhostType::Disabled);
        }
    };

}
