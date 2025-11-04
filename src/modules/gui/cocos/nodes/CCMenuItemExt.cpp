#include "CCMenuItemExt.hpp"

namespace eclipse::gui::cocos {
    CCMenuItemSpriteExtraPro* CCMenuItemSpriteExtraPro::create(
        CCNode* sprite,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        auto ret = new CCMenuItemSpriteExtraPro();
        if (ret->init(sprite, sprite, ret, menu_selector(CCMenuItemSpriteExtraPro::onClick))) {
            ret->m_callback = std::move(callback);
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void CCMenuItemSpriteExtraPro::onClick(CCObject*) {
        m_callback(this);
    }

    CCMenuItemTogglerPro* CCMenuItemTogglerPro::create(
        CCNode* offSprite, CCNode* onSprite, Function<void(CCMenuItemTogglerPro*)>&& callback
    ) {
        auto ret = new CCMenuItemTogglerPro();
        if (ret->init(
            offSprite, onSprite,
            ret, menu_selector(CCMenuItemTogglerPro::onClick)
        )) {
            ret->m_toggleCallback = std::move(callback);
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void CCMenuItemTogglerPro::onClick(CCObject*) {
        m_toggleCallback(this);
    }

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        cocos2d::CCNode* sprite,
        Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        return CCMenuItemSpriteExtraPro::create(sprite, std::move(callback));
    }

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        cocos2d::CCNode* sprite, float scale, Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        sprite->setScale(scale);
        return CCMenuItemSpriteExtraPro::create(sprite, std::move(callback));
    }

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        char const* spriteName, Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        auto sprite = cocos2d::CCSprite::create(spriteName);
        return CCMenuItemSpriteExtraPro::create(sprite, std::move(callback));
    }

    CCMenuItemSpriteExtraPro* createSpriteExtra(
        char const* spriteName, float scale, Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        auto sprite = cocos2d::CCSprite::create(spriteName);
        sprite->setScale(scale);
        return CCMenuItemSpriteExtraPro::create(sprite, std::move(callback));
    }

    CCMenuItemSpriteExtraPro* createSpriteExtraWithFrame(
        char const* spriteFrameName, Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        auto sprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteFrameName);
        return CCMenuItemSpriteExtraPro::create(sprite, std::move(callback));
    }

    CCMenuItemSpriteExtraPro* createSpriteExtraWithFrame(
        char const* spriteFrameName, float scale, Function<void(CCMenuItemSpriteExtraPro*)>&& callback
    ) {
        auto sprite = cocos2d::CCSprite::createWithSpriteFrameName(spriteFrameName);
        sprite->setScale(scale);
        return CCMenuItemSpriteExtraPro::create(sprite, std::move(callback));
    }

    CCMenuItemTogglerPro* createToggler(
        cocos2d::CCNode* offSprite, cocos2d::CCNode* onSprite, Function<void(CCMenuItemTogglerPro*)>&& callback
    ) {
        return CCMenuItemTogglerPro::create(offSprite, onSprite, std::move(callback));
    }
}
