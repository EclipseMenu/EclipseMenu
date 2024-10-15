#include "options-popup.hpp"

#include <modules/gui/gui.hpp>
#include <modules/gui/cocos/cocos.hpp>

#include "content-view.hpp"

namespace eclipse::gui::cocos {
    bool OptionsPopup::setup(std::shared_ptr<MenuTab> const& tab) {
        this->setTitle(tab->getTitle());
        m_title->setPositionY(225.f);

        // Background for content
        m_contentBG = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
        m_contentBG->setAnchorPoint({ 0, 1 });
        m_contentBG->setPosition(7.5f, 210.f);
        m_contentBG->setColor({ 0, 0, 0 });
        m_contentBG->setOpacity(128);
        m_contentBG->setContentSize({ 385.f, 200.f });
        m_contentBG->setID("content-bg"_spr);
        m_mainLayer->addChild(m_contentBG);

        // Content view
        m_contentView = ContentView::create({ 385.f, 200.f }, tab);
        m_contentView->setAnchorPoint({ 0, 1 });
        m_contentView->setPosition(7.5f, 210.f);
        m_mainLayer->addChild(m_contentView, 1);

        // Register the popup with the engine for cleanup
        CocosRenderer::get()->registerOptionsPopup(this);

        return true;
    }

    void OptionsPopup::onExit() {
        Popup::onExit();
        CocosRenderer::get()->unregisterOptionsPopup(this);
    }

    OptionsPopup* OptionsPopup::create(std::shared_ptr<MenuTab> const& tab) {
        auto ret = new OptionsPopup;
        if (ret->initAnchored(400.f, 240.f, std::move(tab))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }
}
