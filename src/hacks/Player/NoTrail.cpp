#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CCMotionStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>

#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_ARM_MAC) || defined(GEODE_IS_IOS)
#define GEODE_IS_ARM 1
#endif

namespace eclipse::hacks::Player {
    class NoTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.notrail")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "No Trail"; }
    };

    REGISTER_HACK(NoTrail)

    class AlwaysTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.alwaystrail")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Always Show Trail"; }
    };

    REGISTER_HACK(AlwaysTrail)

    // CCMotionStreak functions are too small to be hooked on ARM
    #ifdef GEODE_IS_ARM
    class $modify(NoTrailPOHook, PlayerObject) {
        static void onModify(auto& self) {
            SAFE_HOOKS_ALL();
            HOOKS_TOGGLE("player.notrail", PlayerObject, "activateStreak");
            HOOKS_TOGGLE("player.alwaystrail", PlayerObject, "deactivateStreak");
        }

        void deactivateStreak(bool p0) {
            if (!this->m_streakRelated4) return;
            this->m_streakRelated4 = false;
            this->fadeOutStreak2(this->m_playEffects ? 0.2f : 0.6f);
        }

        void activateStreak() {
            if (this->levelFlipping()) return;
            if (utils::get<GameManager>()->m_editorEnabled) return;
            if (this->m_isHidden) return;

            this->m_streakRelated4 = true;
            if (this->m_isDart) {
                auto pos = this->getPosition();
                this->m_waveTrail->m_currentPoint = pos;
                this->m_waveTrail->stopAllActions();
                this->m_waveTrail->setOpacity(255);
                this->m_waveTrail->resumeStroke();
            }
        }
    };
    #else
    class $modify(NoTrailCCMSHook, cocos2d::CCMotionStreak) {
        static void onModify(auto& self) {
            SAFE_HOOKS_ALL();
            HOOKS_TOGGLE("player.notrail", cocos2d::CCMotionStreak, "resumeStroke");
            HOOKS_TOGGLE("player.alwaystrail", cocos2d::CCMotionStreak, "stopStroke");
        }

        void resumeStroke() {}
        void stopStroke() {}
    };
    #endif
}
