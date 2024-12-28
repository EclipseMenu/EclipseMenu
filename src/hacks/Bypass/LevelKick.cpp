#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Bypass {

    class LevelKick : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.levelkick")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Level Kick"; }
    };

    REGISTER_HACK(LevelKick)

    class $modify(LevelKickGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("bypass.levelkick")

        bool shouldExitHackedLevel() { return false; }
    };

}
