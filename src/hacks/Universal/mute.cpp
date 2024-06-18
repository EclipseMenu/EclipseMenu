//from prism its superseded anyways im lazy
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCScheduler.hpp>
#include <Geode/binding/AppDelegate.hpp>
namespace eclipse::hacks::Universal {

    class Mute : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Universal");
            tab->addToggle("Mute", "universal.mute")->setDescription("Toggles sound on/off");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Mute"; }
    };

    REGISTER_HACK(Mute)
    using namespace geode::prelude;
    class $modify(CCScheduler) {
        void update(float dt) {
            if (config::get<bool>("universal.mute", false)) {
                AppDelegate::get()->pauseSound();
            }
            if (!config::get<bool>("universal.mute", false)) {
                AppDelegate::get()->resumeSound();
            }
            CCScheduler::update(dt);
        }
    };

}
