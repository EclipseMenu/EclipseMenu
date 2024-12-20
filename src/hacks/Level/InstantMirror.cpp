#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class InstantMirror : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.instantmirror")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Instant Mirror"; }
    };

    REGISTER_HACK(InstantMirror)

    class $modify(InstantMirrorGJBGLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("level.instantmirror")

        void toggleFlipped(bool p0, bool p1) {
            GJBaseGameLayer::toggleFlipped(p0, true);
        }
    };
}
