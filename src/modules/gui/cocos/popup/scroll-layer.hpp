#pragma once

namespace eclipse::gui::cocos {

    // taken from Object Workshop (a hybrid of robtops cocos and geode)
    class ScrollLayer : public CCScrollLayerExt, public CCScrollLayerExtDelegate {
        protected:
            bool m_touchOutOfBoundary;
            bool m_scrollWheelEnabled;
            cocos2d::CCTouch* m_touchStart;
            cocos2d::CCPoint m_touchStartPosition2;
            cocos2d::CCPoint m_touchPosition2;
            bool m_touchMoved;
            float m_touchLastY;
            bool m_cancellingTouches;

            void cancelAndStoleTouch(cocos2d::CCTouch*, cocos2d::CCEvent*);
            void checkBoundaryOfContent(float);
            void claimTouch(cocos2d::CCTouch*);
            void touchFinish(cocos2d::CCTouch*);

            bool ccTouchBegan(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
            void ccTouchMoved(cocos2d::CCTouch*, cocos2d::CCEvent*) override;

            void ccTouchEnded(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
            void ccTouchCancelled(cocos2d::CCTouch*, cocos2d::CCEvent*) override;
            void scrollWheel(float, float) override;
            void visit() override;
            CCMenuItemSpriteExtra* itemForTouch(cocos2d::CCTouch*);

            ScrollLayer(cocos2d::CCRect const& rect, bool scrollWheelEnabled, bool vertical);
        public:
            void fixTouchPrio();
            void scrollToTop();
            static eclipse::gui::cocos::ScrollLayer* create(
                cocos2d::CCRect const& rect, bool scrollWheelEnabled = true, bool vertical = true
            );
            static eclipse::gui::cocos::ScrollLayer* create(
                cocos2d::CCSize const& size, bool scrollWheelEnabled = true, bool vertical = true
            );
    };
}
