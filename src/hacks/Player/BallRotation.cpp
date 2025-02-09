#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/float-toggle.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $hack(BallRotation) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");

            tab->addToggle("player.ballrotation")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Ball Rotation Bug"; }
    };

    REGISTER_HACK(BallRotation)

    class $modify(BallRotationPOHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.ballrotation")

        void switchedToMode(GameObjectType p0) { // probably the closest i'll get :sob:
            bool prevBall = m_isBall;

            PlayerObject::switchedToMode(p0);

            float fullRot = -360 - this->getRotation();

            cocos2d::CCArray* array = cocos2d::CCArray::create();
            array->addObject(cocos2d::CCDelayTime::create(0.05f));
            array->addObject(cocos2d::CCRotateBy::create(0.85f, fullRot * 0.7f));
            array->addObject(cocos2d::CCEaseExponentialOut::create(cocos2d::CCRotateBy::create(0.1f, fullRot * 0.3f)));

            cocos2d::CCSequence* seq = cocos2d::CCSequence::create(array);

            if (p0 != GameObjectType::BallPortal && prevBall) 
                this->getChildByType<cocos2d::CCNode*>(0)->runAction(seq);
        }
    };
}
