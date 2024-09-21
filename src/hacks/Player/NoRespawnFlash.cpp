#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class NoRespawnFlash : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Respawn Flash", "player.norespawnflash")
                ->setDescription("Removes the blinking effect on respawn.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Respawn Flash"; }
    };

    REGISTER_HACK(NoRespawnFlash)

    class $modify(NoRespawnFlashPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.norespawnflash")
        void playSpawnEffect() {}
    };

}
