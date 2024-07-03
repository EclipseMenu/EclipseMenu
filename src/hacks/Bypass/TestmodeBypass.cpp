#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Bypass {

    class TestmodeBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Testmode Bypass", "bypass.testmode")->setDescription("Hides the \"Testmode\" text in the top left corner when playing from a StartPos");
        }

        [[nodiscard]] const char* getId() const override { return "Testmode Bypass"; }
    };

    REGISTER_HACK(TestmodeBypass)

    class $modify(PlayLayer) {
        void updateTestModeLabel() {
            if (!config::get<bool>("bypass.testmode", false))
                PlayLayer::updateTestModeLabel();
        }
    };

}
