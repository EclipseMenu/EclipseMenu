#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/FMODAudioEngine.hpp>

namespace eclipse::hacks::Level {
    class $hack(NoMusicFadeOut) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.nomusicfadeout")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "No Music Fade Out"; }
    };

    REGISTER_HACK(NoMusicFadeOut)

    class $modify(NoMusicFadeOutFMODAEHook, FMODAudioEngine) {
        ALL_DELEGATES_AND_SAFE_PRIO("level.nomusicfadeout")

        void fadeOutMusic(float duration, int channel) {}
    };
}
