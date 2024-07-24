#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {

    class HidePause : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Hide Pause Menu", "level.hidepause")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Hide Pause Menu"; }
    };

    REGISTER_HACK(HidePause)

    class $modify(HPMPauseLayer, PauseLayer) {
        void customSetup() override {
            HPMPauseLayer::createHideScheduler(this);
            PauseLayer::customSetup();
        }

        static void createHideScheduler(PauseLayer* pauseLayer) {
            pauseLayer->schedule(schedule_selector(HPMPauseLayer::updatePauseMenu));
            pauseLayer->setVisible(!config::get<bool>("level.hidepause", false));
        }

        void updatePauseMenu(float dt) {
            if (config::get<bool>("level.hidepause", false) == this->isVisible())
                this->setVisible(!config::get<bool>("level.hidepause", false));
        }
    };
}