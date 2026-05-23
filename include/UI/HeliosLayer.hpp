#pragma once
#include <cocos2d.h>
#include "../Prelude.hpp"

namespace eclipse {
    class ECLIPSE_DLL HeliosLayer : public cocos2d::CCLayer {
    public:
        static HeliosLayer* get() ECLIPSE_EVENT_METHOD(HeliosLayer::get);

        void toggle() noexcept;
        void setToggled(bool toggled) noexcept;
        [[nodiscard]] bool isToggled() const noexcept { return m_toggled; }

        bool init() override;

        void draw() override;
        void update(float dt) override;

        bool ccTouchBegan(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;
        void ccTouchMoved(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;
        void ccTouchEnded(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;
        void ccTouchCancelled(cocos2d::CCTouch* pTouch, cocos2d::CCEvent* pEvent) override;

        void registerWithTouchDispatcher() override;

    private:
        geode::ListenerHandle m_mouseMoveListener;
        geode::ListenerHandle m_mouseClickListener;
        geode::ListenerHandle m_keyboardListener;

        bool m_toggled = false;
    };
}
