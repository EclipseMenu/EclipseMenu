#pragma once
#include <functional.hpp>

namespace eclipse::gui::cocos {
    class CCMenuItemSpriteExtraPro : public CCMenuItemSpriteExtra {
    public:
        static CCMenuItemSpriteExtraPro* create(
            CCNode* sprite,
            Function<void(CCMenuItemSpriteExtraPro*)>&& callback
        );

        void onClick(CCObject*);

    private:
        Function<void(CCMenuItemSpriteExtraPro*)> m_callback;
    };

    class CCMenuItemTogglerPro : public CCMenuItemToggler {
    public:
        static CCMenuItemTogglerPro* create(
            CCNode* onSprite, CCNode* offSprite,
            Function<void(CCMenuItemTogglerPro*)>&& callback
        );

        void onClick(CCObject*);

    private:
        Function<void(CCMenuItemTogglerPro*)> m_toggleCallback;
    };

    // == CCMenuItemSpriteExtra == //

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        cocos2d::CCNode* sprite,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    );

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        cocos2d::CCNode* sprite, float scale,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    );

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        char const* spriteName,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    );

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        char const* spriteName, float scale,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    );

    CCMenuItemSpriteExtraPro* createSpriteExtraWithFrame(
        char const* spriteFrameName,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    );

    CCMenuItemSpriteExtraPro* createSpriteExtraWithFrame(
        char const* spriteFrameName, float scale,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    );

    // == CCMenuItemToggler == //

    CCMenuItemTogglerPro* createToggler(
        cocos2d::CCNode* onSprite, cocos2d::CCNode* offSprite,
        Function<void(CCMenuItemTogglerPro*)>&& callback
    );
}