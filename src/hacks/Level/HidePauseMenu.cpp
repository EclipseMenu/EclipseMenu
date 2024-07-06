#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {

    class HidePause : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Hide Pause Menu", "level.hidepause")->setDescription("Hides the pause menu. Useful for taking screenshots of levels");
        }

        [[nodiscard]] const char* getId() const override { return "Hide Pause Menu"; }
    };

    REGISTER_HACK(HidePause)

    class $modify(HPMPauseLayer, PauseLayer) {
        bool init(bool p0) {
            if (!PauseLayer::init(p0)) return false;
            this->schedule(schedule_selector(HPMPauseLayer::updatePauseMenu));
            this->setVisible(!config::get<bool>("level.hidepause", false));
            return true;
        }

        void updatePauseMenu(float dt) {
            if (config::get<bool>("level.hidepause", false) == this->isVisible())
                this->setVisible(!config::get<bool>("level.hidepause", false));
        }
    };
}
