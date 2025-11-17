#include "options-popup.hpp"

#include <modules/gui/gui.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <modules/gui/theming/manager.hpp>

#include "content-view.hpp"

namespace eclipse::gui::cocos {
    bool OptionsPopup::setup(MenuTab const& tab) {
        auto const tm = ThemeManager::get();

        auto title = TranslatedLabel::create(tab.getTitle());
        title->setPosition(200, 225);
        title->setID("title"_spr);
        m_mainLayer->addChild(title, 2);

        // The behind background for the entire popup to get the outline
        auto bgBehind = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        bgBehind->setContentSize(m_mainLayer->getContentSize() * std::clamp(tm->getBorderSize(), 0.F, 1.F));
        m_bgSprite->setColor(tm->getBorderColor().toCCColor3B());
        bgBehind->setID("bg-behind"_spr);
        m_mainLayer->addChildAtPosition(bgBehind, geode::Anchor::Center);

        // Background for the entire popup
        m_bgSprite = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        m_bgSprite->setContentSize(m_mainLayer->getContentSize() - 3);
        m_bgSprite->setColor(tm->getTitleBackgroundColor().toCCColor3B());
        m_bgSprite->setID("main-bg"_spr);
        m_mainLayer->addChildAtPosition(m_bgSprite, geode::Anchor::Center);

        // Background for content
        m_contentBG = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        m_contentBG->setAnchorPoint({0, 1});
        m_contentBG->setPosition(7.5f, 210.f);
        m_contentBG->setColor(tm->getBackgroundColor().toCCColor3B());
        //m_contentBG->setOpacity(128);
        m_contentBG->setContentSize({385.f, 200.f});
        m_contentBG->setID("content-bg"_spr);
        m_mainLayer->addChild(m_contentBG);

        // Content view
        m_contentView = ContentView::create({385.f, 200.f}, tab);
        m_contentView->setAnchorPoint({0, 1});
        m_contentView->setPosition(7.5f, 210.f);
        m_mainLayer->addChild(m_contentView, 1);

        // Register the popup with the engine for cleanup
        CocosRenderer::get()->registerModal(this);

        return true;
    }

    void OptionsPopup::onExit() {
        Popup::onExit();
        CocosRenderer::get()->unregisterModal(this);
    }

    OptionsPopup* OptionsPopup::create(MenuTab const& tab) {
        auto ret = new OptionsPopup;
        if (ret->initAnchored(400.f, 240.f, tab)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
}
