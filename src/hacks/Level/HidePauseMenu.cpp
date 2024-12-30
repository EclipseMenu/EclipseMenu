#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Level {
    class HidePause : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.hidepause")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Hide Pause Menu"; }
    };

    REGISTER_HACK(HidePause)

    class $modify(HPMPauseLayerHook, PauseLayer) {
        void customSetup() override {
            HPMPauseLayerHook::createHideScheduler(this);
            PauseLayer::customSetup();
        }

        static void createHideScheduler(PauseLayer* pauseLayer) {
            pauseLayer->schedule(schedule_selector(HPMPauseLayerHook::updatePauseMenu));
            pauseLayer->setVisible(!config::get<bool>("level.hidepause", false));
        }

        void updatePauseMenu(float dt) {
            bool hasZoomMod = geode::Loader::get()->isModLoaded("bobby_shmurner.zoom");

            if (config::get<bool>("level.hidepause", false) == this->isVisible() && (hasZoomMod ? gui::Engine::get()->isToggled() : true)) {
                this->setVisible(!config::get<bool>("level.hidepause", false));
            }
        }
    };
}
