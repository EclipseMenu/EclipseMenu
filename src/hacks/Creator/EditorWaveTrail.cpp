#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/HardStreak.hpp>

namespace eclipse::hacks::Creator { // i didnt do this properly :P

    /*class EditorWaveTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Editor Wave Trail", "creator.editorwave")
                ->handleKeybinds()
                ->setDescription("Enables the wave trail in the Editor");
        }

        [[nodiscard]] const char* getId() const override { return "Editor Wave Trail"; }
    };

    REGISTER_HACK(EditorWaveTrail)

    class $modify(EditorWaveTrailPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.editorwave")

        void placeStreakPoint() {
            if (LevelEditorLayer::get() && m_isDart) m_waveTrail->addPoint(this->getPosition());
            else PlayerObject::placeStreakPoint();
        }

        void fadeOutStreak2(float dt) {
            PlayerObject::fadeOutStreak2(dt);

            if (LevelEditorLayer::get()) m_waveTrail->runAction(cocos2d::CCFadeTo::create(dt, 0));
        }

        virtual void update(float dt) {
            PlayerObject::update(dt);

            if (LevelEditorLayer::get() && m_isDart) m_waveTrail->m_currentPoint = this->getPosition();
        }
    };

    class $modify(EditorWaveTrailHSHook, HardStreak) {
        ADD_HOOKS_DELEGATE("creator.editorwave")

        void updateStroke(float dt) {
            if (LevelEditorLayer::get()) m_drawStreak = true;
            HardStreak::updateStroke(dt);
        }
    };*/
}
