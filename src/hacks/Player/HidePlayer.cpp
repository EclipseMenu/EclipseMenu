#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $hack(HidePlayer) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.hideplayer")->setDescription()->handleKeybinds();
        }
      
        [[nodiscard]] const char* getId() const override { return "Hide Player"; }
    };

    REGISTER_HACK(HidePlayer)
    
    class $modify(PlayerObjectHPHook, PlayerObject){
        ADD_HOOKS_DELEGATE("player.hideplayer")
        void toggleVisibility(bool p0) {
            PlayerObject::toggleVisibility(false);
        }
    };
}
