#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    /*class NoTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Trail", "player.notrail")
                ->handleKeybinds()
                ->setDescription("Force the player trail off.");
        }

        [[nodiscard]] const char* getId() const override { return "No Trail"; }
    };

    REGISTER_HACK(NoTrail)


    class $modify(NoTrailPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.notrail")

        void activateStreak() {}
    };*/
}
