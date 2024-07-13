#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoPracticeMode : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Auto Practice Mode", "level.autopracticemode")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Auto Practice Mode"; }
    };

    REGISTER_HACK(AutoPracticeMode)

    class $modify(PlayLayer) {
        bool init(GJGameLevel* p0, bool p1, bool p2) {
            if (!PlayLayer::init(p0, p1, p2)) return false;

            if (config::get<bool>("level.autopracticemode", false)) {
                PlayLayer::togglePracticeMode(true);
            }

            return true;
        }
    };
}