#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCLightFlash.hpp>

namespace eclipse::hacks::Level {

    class HideLevelCompleteVFX : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Hide Level Complete VFX", "level.hidelevelcomplete")
                ->setDescription("Hides the explosion and fireworks seen when completing a level. (Does not hide particles.) (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Hide Level Complete VFX"; }
    };

    REGISTER_HACK(HideLevelCompleteVFX)


    class $modify(HideLevelCompleteVFXCCCWHook, CCCircleWave) {
        static CCCircleWave* create(float startRadius, float endRadius, float duration, bool fadeIn, bool easeOut) {
            CCCircleWave* cw = CCCircleWave::create(startRadius, endRadius, duration, fadeIn, easeOut);
            PlayLayer* pl = PlayLayer::get();
            if (!pl) return cw;

            if (pl->m_levelEndAnimationStarted && config::get<bool>("level.hidelevelcomplete", false))
                cw->setVisible(false);

            return cw;
        }
    };

    class $modify(HideLevelCompleteVFXCCLFHook, CCLightFlash) {
        // i cant believe i need to hook this function with NINETEEN params to get things working what the heck :despair:
        void playEffect(cocos2d::CCPoint point, cocos2d::ccColor3B color, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, float p10, float p11, float p12, float p13, float p14, float p15, int p16, bool p17, bool p18, float p19) {
            PlayLayer* pl = PlayLayer::get();

            if (!pl)
                return CCLightFlash::playEffect(
                    point, color, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                    p11, p12, p13, p14, p15, p16, p17, p18, p19
                );

            if (pl->m_levelEndAnimationStarted && config::get<bool>("level.hidelevelcomplete", false))
                this->setVisible(false);

            CCLightFlash::playEffect(
                point, color, p2, p3, p4, p5, p6, p7, p8, p9,
                p10, p11, p12, p13, p14, p15, p16, p17, p18, p19
            );
        }
    };
}