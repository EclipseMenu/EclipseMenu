#include "scroll-layer.hpp"
#include "utils.hpp"

#include <modules/utils/SingletonCache.hpp>
// all thanks to https://github.com/CallocGD/GD-2.205-Decompiled
// and also from https://github.com/geode-sdk/geode

namespace eclipse::gui::cocos {
    void ScrollLayer::scrollWheel(float pointX, float pointY) {
        if (pointX != 0.0) {
            CCScrollLayerExt::scrollLayer(pointX);
        }
    }
    bool ScrollLayer::ccTouchBegan(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
        if (geode::cocos::nodeIsVisible(this)) {
            bool value = CCScrollLayerExt::ccTouchBegan(touch, event);
            if (value) {
                m_touchStart = touch;
                auto touchPos = utils::get<cocos2d::CCDirector>()->convertToGL(m_touchStart->getLocationInView());
                m_touchStartPosition2 = touchPos;
                m_touchPosition2 = touchPos;
                if (m_touchOutOfBoundary) {
                    //schedule(schedule_selector(ScrollLayerExt::checkBoundaryOfContent));
                    m_touchOutOfBoundary = true;
                }
                m_touchLastY = m_touchPosition2.y;
                return true;
            }
        }
        return false;
    }
    void ScrollLayer::ccTouchCancelled(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
        if (m_cancellingTouches) return;
        if (m_touchMoved == true) {
            CCScrollLayerExt::ccTouchCancelled(touch, event);
            touchFinish(touch);
            if (m_touchOutOfBoundary != false) {
                //unschedule(schedule_selector(ScrollLayerExt::checkBoundaryOfContent));
                m_touchOutOfBoundary = false;
            }
        }
    }
    void ScrollLayer::ccTouchEnded(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
        CCScrollLayerExt::ccTouchEnded(touch, event);
        touchFinish(touch);
        if (m_cancellingTouches != false) {
            //unschedule(schedule_selector(ScrollLayerExt::checkBoundaryOfContent));
            m_cancellingTouches = false;
        }
    }
    void ScrollLayer::ccTouchMoved(cocos2d::CCTouch *touch, cocos2d::CCEvent *event) {
        CCScrollLayerExt::ccTouchMoved(touch, event);
        m_touchMoved = true;
        auto touchPoint = utils::get<cocos2d::CCDirector>()->convertToGL(touch->getLocationInView());
        if (touch == m_touchStart) {
            m_touchPosition2 = m_touchPosition2 - touchPoint;
        }
        if (fabsf(touchPoint.y - m_touchLastY) >= 10.F) {
            m_touchLastY = touchPoint.y;
            cancelAndStoleTouch(touch, event);
        }
    }
    void ScrollLayer::claimTouch(cocos2d::CCTouch* touch) {
        auto touchDispatcher = utils::get<cocos2d::CCDirector>()->getTouchDispatcher();
        auto handler = static_cast<cocos2d::CCTargetedTouchHandler *>(touchDispatcher->findHandler(this));
        if (handler) {
            cocos2d::CCSet* claimedTouches = handler->getClaimedTouches();
            if (!claimedTouches->containsObject(touch)) {
                claimedTouches->addObject(touch);
            }
        }
    }
    void ScrollLayer::cancelAndStoleTouch(cocos2d::CCTouch *touch, cocos2d::CCEvent *event)
    {
        cocos2d::CCSet* set = new cocos2d::CCSet();
        set->addObject(touch);
        set->autorelease();
        m_cancellingTouches = true;
        auto touchDispather = utils::get<cocos2d::CCDirector>()->getTouchDispatcher();
        touchDispather->touchesCancelled(set, event);
        m_cancellingTouches = false;
        claimTouch(touch);
        /*
      piVar1 = (int *)cocos2d::CCDirector::sharedDirector();
      piVar1 = (int *)(**(code **)(*piVar1 + 0x50))();
      (**(code **)(*piVar1 + 0x34))(piVar1,this_00,param_2);
      this->m_cancellingTouches = false;
      claimTouch(this,param_1);
        */
    }


    ScrollLayer::ScrollLayer(cocos2d::CCRect const& rect, bool scrollWheelEnabled, bool vertical) :
        CCScrollLayerExt(rect) {
        m_scrollWheelEnabled = scrollWheelEnabled;

        m_disableVertical = !vertical;
        m_disableHorizontal = vertical;
        m_cutContent = true;

        m_contentLayer->removeFromParent();
        m_contentLayer = geode::GenericContentLayer::create(rect.size.width, rect.size.height);
        m_contentLayer->setID("content-layer");
        m_contentLayer->setAnchorPoint({ 0, 0 });
        this->addChild(m_contentLayer);

        m_touchStartPosition2 = cocos2d::CCPointMake(0.f, 0.f);
        m_touchPosition2 = cocos2d::CCPointMake(0.f, 0.f);

        this->setID("ScrollLayerExt"_spr);

        this->setMouseEnabled(true);
        this->setTouchEnabled(true);

        utils::get<cocos2d::CCTouchDispatcher>()->registerForcePrio(this, 2);
    }

    ScrollLayer::~ScrollLayer() {
        utils::get<cocos2d::CCTouchDispatcher>()->unregisterForcePrio(this);
    }

    void ScrollLayer::visit() {
        int previousRect[4];
        bool previousScissor = glIsEnabled(GL_SCISSOR_TEST);

        if (m_cutContent && this->isVisible()) {
            if (previousScissor) {
                glGetIntegerv(GL_SCISSOR_BOX, previousRect);
            }
            else {
                glEnable(GL_SCISSOR_TEST);
            }

            if (this->getParent()) {
                auto const bottomLeft = this->convertToWorldSpace(ccp(0, 0));
                auto const topRight = this->convertToWorldSpace(this->getContentSize());
                cocos2d::CCSize const size = topRight - bottomLeft;

                utils::get<cocos2d::CCEGLView>()->setScissorInPoints(bottomLeft.x, bottomLeft.y, size.width, size.height);
            }
        }

        CCNode::visit();

        if (m_cutContent && this->isVisible()) {
            if (previousScissor) {
                glScissor(previousRect[0], previousRect[1], previousRect[2], previousRect[3]);
            }
            else {
                glDisable(GL_SCISSOR_TEST);
            }
        }
    }

    void ScrollLayer::scrollToTop() {
        auto listTopScrollPos = -m_contentLayer->getContentHeight() + this->getContentHeight();
        m_contentLayer->setPositionY(listTopScrollPos);
    }
    ScrollLayer* ScrollLayer::create(cocos2d::CCRect const& rect, bool scroll, bool vertical) {
        auto ret = new ScrollLayer(rect, scroll, vertical);
        ret->autorelease();

        return ret;
    }
    ScrollLayer* ScrollLayer::create(cocos2d::CCSize const& size, bool scroll, bool vertical) {
        return ScrollLayer::create({ 0, 0, size.width, size.height }, scroll, vertical);
    }
    void ScrollLayer::touchFinish(cocos2d::CCTouch* touch) {
        auto touchPoint = utils::get<cocos2d::CCDirector>()->convertToGL(touch->getLocationInView());
        if (touch == this->m_touchStart) {
            //auto pvVar1 = (CCMenuItemSpriteExtra *)itemForTouch((CCTouch *)this);
            auto pvVar1 = nullptr;
            if (pvVar1 != nullptr && !m_notAtEndOfScroll) {
            }
        }
        m_touchStartPosition2 = m_touchPosition2;
        m_touchMoved = false;
    }

    void ScrollLayer::registerWithTouchDispatcher() {
        utils::get<cocos2d::CCDirector>()->getTouchDispatcher()->addPrioTargetedDelegate(this, -500, false);
    }
}
