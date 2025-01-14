#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(PauseDuringComplete) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.pauseduringcomplete")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Pause During Complete"; }
    };

    REGISTER_HACK(PauseDuringComplete)

    class $modify(PauseDuringCompletePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.pauseduringcomplete")

        void pauseGame(bool p0) {
            bool original = m_levelEndAnimationStarted;
            m_levelEndAnimationStarted = false;
            PlayLayer::pauseGame(p0);
            m_levelEndAnimationStarted = original;
        }
    };
}
