#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#ifdef GEODE_IS_WINDOWS

namespace eclipse::hacks::Global {
    class CBFToggle : public hack::Hack {
        void init() override {}

        void lateInit() override {
            auto cbf = geode::Loader::get()->getLoadedMod("syzzi.click_between_frames");
            if (!cbf) return; // mod not loaded

            auto tab = gui::MenuTab::find("tab.global");
            tab->addToggle("global.click-between-frames", "syzzi.click_between_frames.toggle")
                ->handleKeybinds()
                ->setDescription()
                ->callback([cbf](bool v){
                    // soft-toggle means disable if true, so we invert the value
                    cbf->setSettingValue<bool>("soft-toggle", !v);
                })->disableSaving();

            listenForSettingChanges<bool>("soft-toggle", [](bool v) {
                config::setTemp("syzzi.click_between_frames.toggle", !v);
            }, cbf);

            config::setTemp("syzzi.click_between_frames.toggle", !cbf->getSettingValue<bool>("soft-toggle"));
        }

        [[nodiscard]] const char* getId() const override { return "Click Between Frames"; }
    };

    REGISTER_HACK(CBFToggle)
}
#endif