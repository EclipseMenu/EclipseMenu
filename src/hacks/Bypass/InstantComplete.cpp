#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Bypass {

    class InstantComplete : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Instant Complete", "bypass.instantcomplete")->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("bypass.instantcomplete", false); }
        [[nodiscard]] const char* getId() const override { return "Instant Complete"; }
    };

    REGISTER_HACK(InstantComplete)

    class $modify(PlayLayer) {
        bool init(GJGameLevel *gj, bool p1, bool p2) {
            if (!PlayLayer::init(gj, p1, p2)) return false;

            // TODO: this causes roberts ac to trigger
            if (config::get<bool>("bypass.instantcomplete", false))
                PlayLayer::playEndAnimationToPos({2,2});

            return true;
        }
    };
}
