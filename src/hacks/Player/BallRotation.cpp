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

            tab->addToggle("player.ballrotation")
               ->handleKeybinds()
               ->setDescription()
               ->addOptions([](auto options) {
                   options->addToggle("player.ballrotation.variation")->setDescription();
               });
        }

        [[nodiscard]] const char* getId() const override { return "Ball Rotation Bug"; }
    };

    REGISTER_HACK(BallRotation)

    class BallRotationAction : public cocos2d::CCAction {
    public:
        static BallRotationAction* create(cocos2d::CCActionInterval* action) {
            auto ret = new BallRotationAction();
            ret->m_innerAction = action;
            action->retain();
            ret->autorelease();
            return ret;
        }

        void step(float dt) override {
            m_innerAction->step(dt);

            if (m_innerAction->isDone()) {
                if (m_extraNode && m_madeExtraTurn) {
                    m_extraNode->setRotation(m_originalRotation);
                    return;
                }
                if (!m_madeExtraTurn) {
                    // restart the action
                    float diff = m_innerAction->getElapsed() - m_innerAction->getDuration();
                    m_innerAction->startWithTarget(m_pTarget);
                    m_innerAction->step(0.0f);
                    m_innerAction->step(diff);
                    m_madeExtraTurn = m_extraNode != nullptr;
                }
            }

            if (m_extraNode) {
                m_extraNode->setRotation(m_pTarget->getRotation());
            }
        }

        bool isDone() override {
            return m_innerAction->isDone() && m_extraNode;
        }

        void beginExtraAction(cocos2d::CCNode* node) {
            m_extraNode = node;
            m_originalRotation = node->getRotation();
            m_madeExtraTurn = false;
        }

    protected:
        cocos2d::CCActionInterval* m_innerAction;
        cocos2d::CCNode* m_extraNode = nullptr;
        float m_originalRotation = 0;
        bool m_madeExtraTurn = false;
    };

    class $modify(BallRotationPOHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.ballrotation")

        struct Fields {
            BallRotationAction* m_spinAction = nullptr;
            geode::Ref<CCNode> m_spinNode;

            Fields() {
                m_spinNode = CCNode::create();
                m_spinNode->onEnter(); // enables scheduler for the dummy node
            }
        };

        void switchedToMode(GameObjectType p0) { // probably the closest i'll get :sob:
            bool prevBall = m_isBall;
            PlayerObject::switchedToMode(p0);

            if (p0 != GameObjectType::BallPortal && prevBall) {
                if (config::get<bool>("player.ballrotation.variation", false)) {
                    auto fields = m_fields.self();
                    if (fields->m_spinAction) {
                        fields->m_spinAction->beginExtraAction(m_mainLayer);
                        fields->m_spinAction = nullptr;
                    }
                } else {
                    float fullRot = -360 - this->getRotation();
                    m_mainLayer->runAction(cocos2d::CCSequence::create(
                        cocos2d::CCDelayTime::create(0.05f),
                        cocos2d::CCRotateBy::create(0.85f, fullRot * 0.7f),
                        cocos2d::CCEaseExponentialOut::create(cocos2d::CCRotateBy::create(
                            0.1f, fullRot * 0.3f
                        )),
                        nullptr
                    ));
                }
            }
        }

        void prepareForRotation(float mult) {
            if (!config::get<bool>("player.ballrotation.variation", false)) {
                return;
            }

            auto flipMod = m_isUpsideDown ? -1 : 1;
            auto reverseMod = m_isGoingLeft ? -1 : 1;
            auto sidewaysMod = m_isSideways ? -1 : 1;

            auto numerator = 120 * flipMod * reverseMod * sidewaysMod;
            auto denom = numerator / m_rotationSpeed;

            auto fields = m_fields.self();

            // calling action->stop() is apparently not safe
            if (fields->m_spinAction) {
                fields->m_spinAction->getTarget()->stopAction(fields->m_spinAction);
            }

            fields->m_spinAction = BallRotationAction::create(
                cocos2d::CCRotateBy::create(denom, numerator * mult)
            );

            fields->m_spinNode->setRotation(m_mainLayer->getRotation());
            fields->m_spinNode->runAction(fields->m_spinAction);
        }

        /* Sometimes you just gotta love macOS for the amount of inlines it has */
        #ifdef GEODE_IS_MACOS
        void runRotateAction(bool p0, int p1) {
            bool willRun = !this->m_isLocked && !this->m_isDashing && this->m_isBall;
            PlayerObject::runRotateAction(p0, p1);
            if (willRun) this->prepareForRotation(1.f);
        }

        void stopDashing() {
            bool willRun = this->m_isDashing && this->m_isBall;
            PlayerObject::stopDashing();
            if (willRun) this->prepareForRotation(1.f);
        }
        #else
        void runBallRotation(float p0) {
            PlayerObject::runBallRotation(p0);
            this->prepareForRotation(p0);
        }
        #endif
    };
}
