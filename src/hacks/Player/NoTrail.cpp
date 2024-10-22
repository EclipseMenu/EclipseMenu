#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCMotionStreak.hpp>

namespace eclipse::hacks::Player {

    // why was this commented out? anyways i just took some code from ninny's mod
    class NoTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("No Trail", "player.notrail")
                ->handleKeybinds()
                ->setDescription("Force the player trail off.");
        }

        [[nodiscard]] const char* getId() const override { return "No Trail"; }
    };

    REGISTER_HACK(NoTrail)


    class $modify(NoTrailCCHook, cocos2d::CCMotionStreak) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.notrail")
        void resumeStroke() {}
    };

    class AlwaysTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Always Show Trail", "player.alwaystrail")
                ->handleKeybinds()
                ->setDescription("Always shows the trail.");
        }

        [[nodiscard]] const char* getId() const override { return "Always Show Trail"; }
    };

    REGISTER_HACK(AlwaysTrail)


    class $modify(AlwaysTrailCCHook, cocos2d::CCMotionStreak) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.alwaystrail")
        void stopStroke() {}
    };
}
