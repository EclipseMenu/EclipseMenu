#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class ZeroPercentPractice : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("0% Practice Completion", "level.zeropercentpractice")
                ->handleKeybinds()
                ->setDescription("Treats completions from 0% in Practice Mode as Normal Mode completions");
        }

        [[nodiscard]] const char* getId() const override { return "0% Practice Completion"; }
    };

    REGISTER_HACK(ZeroPercentPractice)

    class $modify(ZeroPercentPracticePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.zeropercentpractice")

        struct Fields {
            bool fromZero = true;
        };

        void resetLevel() {
            m_fields->fromZero = m_gameState.m_currentProgress == 0;
            PlayLayer::resetLevel();
        }

        void levelComplete() {
            if (m_fields->fromZero && m_isPracticeMode) m_isPracticeMode = false;
            PlayLayer::levelComplete();
        }
    };
}
