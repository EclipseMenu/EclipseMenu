#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {

    class HidePause : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Hide Pause Menu", "level.hidepause");
        }

        [[nodiscard]] const char* getId() const override { return "Hide Pause Menu"; }
    };

    REGISTER_HACK(HidePause)

    class $modify(HPMPauseLayer, PauseLayer) {
        static PauseLayer* create(bool b) {
            PauseLayer* pm = PauseLayer::create(b);
            pm->schedule(schedule_selector(HPMPauseLayer::updatePauseMenu));
            return pm;
        }

        void updatePauseMenu(float dt) {
            if (config::get<bool>("level.hidepause", false) == this->isVisible()) this->setVisible(!config::get<bool>("level.hidepause", false));
        }
    };
}