#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoPracticeMode : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Auto Practice Mode", "level.autopracticemode")
                ->handleKeybinds()
                ->setDescription("Automatically enables practice mode when entering a level. (Created by Uproxide)");
        }

        [[nodiscard]] const char* getId() const override { return "Auto Practice Mode"; }
    };

    REGISTER_HACK(AutoPracticeMode)

    class $modify(AutoPracticeModePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.autopracticemode")

        bool init(GJGameLevel* p0, bool p1, bool p2) {
            if (!PlayLayer::init(p0, p1, p2))
                return false;

            this->togglePracticeMode(true);
            return true;
        }
    };
}
