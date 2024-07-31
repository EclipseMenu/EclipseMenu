#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class NoGhostTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Ghost Trail", "player.noghosttrail")
                ->setDescription("Disables player ghost trail triggers.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Ghost Trail"; }
    };

    REGISTER_HACK(NoGhostTrail)

    class $modify(NoGhostTrailPOHook, PlayerObject) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("PlayerObject::toggleGhostEffect");
        }

        void toggleGhostEffect(GhostType type) {
            if (config::get<bool>("player.noghosttrail", false))
                type = GhostType::Disabled;

            PlayerObject::toggleGhostEffect(type);
        }
    };

}
