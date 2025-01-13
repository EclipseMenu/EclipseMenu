#pragma once
#include <cocos2d.h>

#if defined(GEODE_IS_MOBILE) // || defined(ECLIPSE_DEBUG_BUILD)
#define ECLIPSE_USE_FLOATING_BUTTON
#endif

namespace eclipse::gui {

    class FloatingButton : public cocos2d::CCMenu {
    protected:
        // how many units required to begin dragging the button
        constexpr static float MIN_MOVE_DISTANCE = 5.f;
        // move progress per second
        constexpr static float MOVE_SPEED = 9.75f;
        // how many units until the button snaps to touch pos
        constexpr static float SNAP_MARGIN = 0.1f;

        std::function<void()> m_callback;
        cocos2d::CCSprite* m_sprite{}; // "main-sprite"
        cocos2d::CCPoint m_holdPosition{}; // last cursor/touch position
        float m_minOpacity = 0.2f;
        float m_maxOpacity = 0.9f;
        float m_postClickTimer = 0.f; // determines beginning of fadeOut after click
        bool m_showInLevel = false;
        bool m_showInEditor = false;
        bool m_shouldMove = false; // whether currently in move animation
        bool m_haveMoved = false; // used to determine whether we should trigger callback after release
        bool m_haveReleased = false;

    public:
        static FloatingButton* get();
        void setCallback(std::function<void()> callback);

        void setScale(float scale) override;

    protected:
        static FloatingButton* create();
        bool init() override;
        void update(float) override;

        bool shouldHide() const;
        float getRadius() const;
        void fadeIn() const;
        void fadeOut() const;

        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
            this->ccTouchEnded(touch, event);
        }

        void registerWithTouchDispatcher() override;

    };


}