#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCCircleWave.hpp>
#include <Geode/modify/CCLightFlash.hpp>
#include <Geode/modify/CCParticleSystem.hpp>

namespace eclipse::hacks::Level {
    class HideLevelCompleteVFX : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Hide Level Complete VFX", "level.hidelevelcomplete")
                ->setDescription("Hides the explosion, fireworks, and particles seen when completing a level. (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Hide Level Complete VFX"; }
    };

    REGISTER_HACK(HideLevelCompleteVFX)

    class $modify(HideLevelCompleteVFXCCCWHook, CCCircleWave) {
        void setPosition(cocos2d::CCPoint const& p0) {
            CCCircleWave::setPosition(p0);

            if (!config::get<bool>("level.hidelevelcomplete", false)) return;

            PlayLayer* pl = PlayLayer::get();

            if (!pl) return;

            /*
            CCCircleWaves sometimes get added
            as child of CurrencyRewardLayer when completing rated levels
            so i had to change hooks to CCCircleWave::draw();

            this doesnt affect CCCircleWaves when playlayer is active
            thanks to the m_levelEndAnimationStarted member variable
            */
            if (pl->m_levelEndAnimationStarted && !geode::cast::typeinfo_cast<CurrencyRewardLayer*>(this->getParent()))
                this->setVisible(false);
        }
    };

    class $modify(HideLevelCompleteVFXCCLFHook, CCLightFlash) {
        // i cant believe i need to hook this function with TWENTY params to get things working what the heck :despair:
        void playEffect(cocos2d::CCPoint point, cocos2d::ccColor3B color, float p2, float p3, float p4, float p5, float p6, float p7, float p8, float p9, float p10, float p11, float p12, float p13, float p14, float p15, int p16, bool p17, bool p18, float p19) {
            CCLightFlash::playEffect(
                point, color, p2, p3, p4, p5, p6, p7, p8, p9, p10,
                p11, p12, p13, p14, p15, p16, p17, p18, p19
            );

            if (!config::get<bool>("level.hidelevelcomplete", false)) return;

            PlayLayer* pl = PlayLayer::get();

            if (!pl) return;

            if (pl->m_levelEndAnimationStarted)
                this->setVisible(false);
        }
    };

    // cannot hook the class below due to macos arm/intel differences (?)
    // nvm it was some setupVBO() issue

    class $modify(HideLevelCompleteVFXCCPSQHook, cocos2d::CCParticleSystemQuad) {
        static cocos2d::CCParticleSystemQuad* create(char const* p0, bool p1) {
            auto psq = cocos2d::CCParticleSystemQuad::create(p0, p1);
            if (!config::get<bool>("level.hidelevelcomplete", false)) return psq;
            PlayLayer* pl = PlayLayer::get();
            if (!pl) return psq;
            if (psq->getParent() != pl) return psq;

            if (pl->m_levelEndAnimationStarted)
                psq->setVisible(false);

            return psq;
        }
    };
    /*
    class $modify(HideLevelCompleteVFXCCPSHook, cocos2d::CCParticleSystem) {
        void update(float dt) {
            CCParticleSystem::update(dt);

            if (!config::get<bool>("level.hidelevelcomplete", false)) return;

            PlayLayer* pl = PlayLayer::get();

            if (!pl) return;
            if (this->getParent() != pl) return;
            if (!geode::cast::typeinfo_cast<cocos2d::CCParticleSystemQuad*>(this)) return;

            if (pl->m_levelEndAnimationStarted)
                this->setVisible(false);
        }
    };
    */
}