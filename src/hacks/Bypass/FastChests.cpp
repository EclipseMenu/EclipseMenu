#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/RewardUnlockLayer.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(FastChests) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.fastchests")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Fast Chests"; }
    };

    REGISTER_HACK(FastChests)

    class $modify(FastChestsRWLHook, RewardUnlockLayer) {
        ADD_HOOKS_DELEGATE("bypass.fastchests")

        bool init(int chestType, RewardsPage* rewardsPage) {
            if (!RewardUnlockLayer::init(chestType, rewardsPage))
                return false;

            auto winSize = utils::get<cocos2d::CCDirector>()->getWinSize();

            m_chestSprite->stopAllActions();
            m_chestSprite->runAction(
                cocos2d::CCEaseBounceOut::create(
                    cocos2d::CCMoveTo::create(
                        1.0, { winSize.width / 2, (winSize.height / 2) - 20 }
                    )
                )
            );
            m_chestSprite->runAction(
                cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(0.4),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::step3)),
                    nullptr
                )
            );

            return true;
        }
    };
}
