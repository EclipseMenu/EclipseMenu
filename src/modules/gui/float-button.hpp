#pragma once
#include <cocos2d.h>
#include <functional.hpp>

#if defined(GEODE_IS_MOBILE) // || defined(ECLIPSE_DEBUG_BUILD)
#define ECLIPSE_USE_FLOATING_BUTTON
#endif

namespace eclipse::gui {

    class FloatingButton : public cocos2d::CCMenu {
    protected:
        // how many units required to begin dragging the button
        constexpr static float MIN_MOVE_DISTANCE = 5.f;
        // move progress per second
        constexpr static float MOVE_SPEED = 10.75f;
        // how many units until the button snaps to touch pos
#ifndef GEODE_IS_IOS
        constexpr static float SNAP_MARGIN = 0.1f;
#else
        // TODO: change this as this may not be the right value for iOS! (it wont move unless the value is big)
        constexpr static float SNAP_MARGIN = 40.f;
#endif
        // scale when pressed
        constexpr static float PRESS_SCALE = 0.9f;

        Function<void()> m_callback;
        cocos2d::CCSprite* m_sprite{}; // "main-sprite"
        cocos2d::CCPoint m_holdPosition{}; // last cursor/touch position
        float m_minOpacity = 0.2f;
        float m_maxOpacity = 0.9f;
        float m_postClickTimer = 0.f; // determines beginning of fadeOut after click
        float m_baseScale = 0.25f; // base scale of the sprite
        bool m_showInLevel = false;
        bool m_showInEditor = false;
        bool m_shouldMove = false; // whether currently in move animation
        bool m_haveMoved = false; // used to determine whether we should trigger callback after release
        bool m_haveReleased = false;

    public:
        static FloatingButton* get();
        void setCallback(Function<void()>&& callback);

        void setScale(float scale) override;

        void reloadSprite();

    protected:
        static FloatingButton* create();
        cocos2d::CCSprite* createSprite() const;
        bool init() override;
        void update(float) override;

        bool shouldHide() const;
        float getRadius() const;
        void fadeIn() const;
        void fadeOut() const;
        void scaleDown();
        void scaleUp();

        bool ccTouchBegan(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchEnded(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override;
        void ccTouchCancelled(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
            this->ccTouchEnded(touch, event);
        }

        void registerWithTouchDispatcher() override;

        ~FloatingButton() override;

    };


}
