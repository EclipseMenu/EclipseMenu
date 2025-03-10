#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class $hack(NoMirror) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.nomirror")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Mirror"; }
    };

    REGISTER_HACK(NoMirror)

    class $modify(NoMirrorGJBGLHook, GJBaseGameLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("level.nomirror")

        void toggleFlipped(bool, bool) {}
    };
}