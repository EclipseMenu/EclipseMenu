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

        bool init(int chestType, RewardsPage* rewardsPage) { // a rough recreation* of this func, because i dont wanna do patching :D
            // NEON_fmadd(a,b,c) = (a * b) + c
            if (!FLAlertLayer::init(0)) return false;
            m_backgroundObjects = cocos2d::CCArray::create();
            m_backgroundObjects->retain();
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            bool bVar1 = 5 < chestType;
            auto spriteBG = cocos2d::extension::CCScale9Sprite::create((bVar1) ? "GJ_square06.png" : "GJ_square02.png", {0, 0, 80, 80});
            spriteBG->setContentSize({340.0,220.0});
            spriteBG->setPosition(winSize / 2);
            m_mainLayer->addChild(spriteBG,0xffffffff);
            spriteBG->runAction(cocos2d::CCFadeIn::create(0.2));

            cocos2d::ccColor3B colorBG = {255, 255, 255};
            float hueShift = 0.f;
            switch (chestType) {
                case 6:
                    colorBG = {13, 68, 104};
                    break;
                case 7:
                    colorBG = {51, 101, 74};
                    hueShift = -50.f;
                    break;
                case 8:
                    colorBG = {73, 34, 94};
                    hueShift = 80.f;
                    break;
            }

            spriteBG->setColor(colorBG);
            m_backgroundObjects->addObject(spriteBG);
            if (bVar1) {
                auto spriteBG2 = cocos2d::extension::CCScale9Sprite::create("GJ_square07.png", {0, 0, 80, 80});
                spriteBG2->setContentSize({340.0,220.0});
                spriteBG2->setPosition(winSize / 2);
                m_mainLayer->addChild(spriteBG2,0xfffffffe);
                spriteBG2->runAction(cocos2d::CCFadeIn::create(0.2));
                m_backgroundObjects->addObject(spriteBG2);
            }
            this->runAction(cocos2d::CCFadeTo::create(0.2F, 150));
            // ghidra says do while loop, i say for loop
            for (size_t i = 0; i < 4; i++) {
                auto spriteHue = CCSpriteWithHue::createWithSpriteFrameName("rewardCorner_001.png");
                m_mainLayer->addChild(spriteHue, 8);
                m_backgroundObjects->addObject(spriteHue);
                cocos2d::CCPoint offsetPoint;
                if (bVar1) {
                    spriteHue->setHueDegrees(hueShift);
                }
                // ok i dont know if its ghidra being like that, but cant you just use setFlipX or setFlipY? theyre the same thing...
                // also yes i couldve done switch but it looks uglier than this
                if (i == 0) {
                    offsetPoint.x = -145.F;
                    offsetPoint.y = -85.F;
                } else if (i == 1) {
                    spriteHue->setScaleY(-spriteHue->getScaleY());
                    offsetPoint.x = -145.F;
                    offsetPoint.y = 85.F;
                } else if (i == 2) {
                    /*
                    pcVar32 = spriteHue->setScaleX;
                    fVar36 = spriteHue->getScaleX();
                    (*pcVar32)(-fVar36,this_02);
                    pcVar32 = spriteHue->setScaleY;
                    fVar36 = spriteHue->getScaleY();
                    (*pcVar32)(-fVar36,this_02);
                    fVar35 = 145;
                    fVar37 = 85;
                    */
                    spriteHue->setScale(-spriteHue->getScale());
                    offsetPoint.x = 145.F;
                    offsetPoint.y = 85.F;
                } else if (i == 3) {
                    spriteHue->setScaleX(-spriteHue->getScaleX());
                    offsetPoint.x = 145.F;
                    offsetPoint.y = -85.F;
                }
                spriteHue->setPosition((winSize / 2) + offsetPoint);
                spriteHue->runAction(cocos2d::CCFadeIn::create(0.2));
            }
            m_rewardsPage = rewardsPage;
            if (rewardsPage != nullptr) {
                m_rewardsPage->retain();
            }
            m_chestType = chestType;
            m_chestSprite = GJChestSprite::create(chestType);
            m_mainLayer->addChild(m_chestSprite, 2);
            m_chestSprite->runAction(
                cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(0.4),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::step3)), // instead of step2, we skip to step3
                    nullptr
                )
            );
            m_chestSprite->setPosition({winSize.width / 2, (winSize.height / 2) - 20});
            m_closeBtn = CCMenuItemSpriteExtra::create(cocos2d::CCSprite::createWithSpriteFrameName("GJ_deleteBtn_001.png"), this, menu_selector(RewardUnlockLayer::onClose));
            m_closeBtn->setSizeMult(1.6F);
            m_rewardBtn = CCMenuItemSpriteExtra::create(cocos2d::CCSprite::createWithSpriteFrameName("GJ_rewardBtn_001.png"), this, menu_selector(RewardUnlockLayer::onClose));
            m_rewardBtn->setSizeMult(1.6F);
            m_buttonMenu = cocos2d::CCMenu::create();
            m_buttonMenu->addChild(m_closeBtn);
            m_buttonMenu->addChild(m_rewardBtn);
            m_mainLayer->addChild(m_buttonMenu, 10);
            m_closeBtn->setEnabled(false);
            m_closeBtn->setVisible(false);
            m_rewardBtn->setEnabled(false);
            m_rewardBtn->setVisible(false);
            cocos2d::CCPoint cornerPos(((winSize.width / 2) - 170) + 10, ((winSize.height / 2) + 110) - 10);
            m_closeBtn->setPosition(m_buttonMenu->convertToNodeSpace(cornerPos));
            m_wrongLabel = cocos2d::CCLabelBMFont::create("Something went wrong...","goldFont.fnt");
            m_mainLayer->addChild(m_wrongLabel);
            m_wrongLabel->setScale(0.6F);
            m_wrongLabel->setPosition((winSize.width / 2), (winSize.height / 2) - 90);
            m_wrongLabel->setVisible(false);
            this->runAction(
                cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(10.0),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(RewardUnlockLayer::connectionTimeout)),
                    nullptr
                )
            );
            return true;
        }
    };
}
