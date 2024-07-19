#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoPractice : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Auto Practice", "level.autoprac")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Auto Practice"; }
    };

    REGISTER_HACK(AutoPractice)

    class $modify(PlayLayer) {
        bool init(GJGameLevel* gj, bool p0, bool p1) {
            if (!PlayLayer::init(gj, p0, p1)) return false;

            if (config::get<bool>("level.autoprac")) this->togglePracticeMode(true);

            return true;
        }
    };
}