#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class $hack(WaveTrailOnDeath) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.wavetrailondeath")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Wave Trail on Death"; }
    };

    REGISTER_HACK(WaveTrailOnDeath)

    class $modify(WaveTrailOnDeathPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.wavetrailondeath")
        void fadeOutStreak2(float duration) {}
        void createFadeOutDartStreak() {}
    };

}
