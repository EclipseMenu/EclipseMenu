#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class PauseDuringComplete : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Pause During Complete", "level.pauseduringcomplete")
                ->setDescription("Allows you to pause during the level complete cutscene.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Pause During Complete"; }
    };

    REGISTER_HACK(PauseDuringComplete)

    class $modify(PauseDuringComplete_Hook, PlayLayer) {
        void pauseGame(bool p0) {
            if (!config::get<bool>("level.pauseduringcomplete", false))
                return PlayLayer::pauseGame(p0);

            bool original = m_levelEndAnimationStarted;
            m_levelEndAnimationStarted = false;
            PlayLayer::pauseGame(p0);
            m_levelEndAnimationStarted = original;
        }
    };
}