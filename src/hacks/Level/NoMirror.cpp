#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class NoMirror : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("No Mirror", "level.nomirror")
                ->handleKeybinds()
                ->setDescription("Disables mirror portals");
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("level.nomirror", false); }
        [[nodiscard]] const char* getId() const override { return "No Mirror"; }
    };

    REGISTER_HACK(NoMirror)

    class $modify(GJBaseGameLayer) {
        void toggleFlipped(bool p0, bool p1) {
            if (!config::get<bool>("level.nomirror", false))
                GJBaseGameLayer::toggleFlipped(p0, p1);
        }
    };
}