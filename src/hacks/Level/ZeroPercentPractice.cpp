#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ZeroPercentPractice) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.zeropercentpractice")->handleKeybinds()->setDescription();
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
