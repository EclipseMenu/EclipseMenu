#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Creator { // i didnt do this properly :P

    /*class $hack(EditorWaveTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.editorwave")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Editor Wave Trail"; }
    };

    REGISTER_HACK(EditorWaveTrail)

    class $modify(EditorWaveTrailPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.editorwave")

        void placeStreakPoint() {
            if (utils::get<LevelEditorLayer>() && m_isDart) m_waveTrail->addPoint(this->getPosition());
            else PlayerObject::placeStreakPoint();
        }

        void fadeOutStreak2(float dt) {
            PlayerObject::fadeOutStreak2(dt);

            if (utils::get<LevelEditorLayer>()) m_waveTrail->runAction(cocos2d::CCFadeTo::create(dt, 0));
        }

        virtual void update(float dt) {
            PlayerObject::update(dt);

            if (utils::get<LevelEditorLayer>() && m_isDart) m_waveTrail->m_currentPoint = this->getPosition();
        }
    };

    class $modify(EditorWaveTrailHSHook, HardStreak) {
        ADD_HOOKS_DELEGATE("creator.editorwave")

        void updateStroke(float dt) {
            if (utils::get<LevelEditorLayer>()) m_drawStreak = true;
            HardStreak::updateStroke(dt);
        }
    };*/
}
