#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/FMODAudioEngine.hpp>

namespace eclipse::hacks::Global {
    class $hack(NoMusicFadeOut) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");
            tab->addToggle("global.nomusicfadeout")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "No Music Fade Out"; }
    };

    REGISTER_HACK(NoMusicFadeOut)

    class $modify(NMFOFMODAudioEngineHook, FMODAudioEngine) {
        ADD_HOOKS_DELEGATE("global.nomusicfadeout")

        void fadeOutMusic(float duration, int channel) {
            return;
        }
    };
}