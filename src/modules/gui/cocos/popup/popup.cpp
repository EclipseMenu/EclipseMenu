#include "popup.hpp"
#include <modules/gui/cocos/cocos.hpp>
#include <modules/config/config.hpp>

#include <utility>

#include "content-view.hpp"

namespace eclipse::gui::cocos {

    bool Popup::setup(Tabs const& tabs) {
        auto winSize = cocos2d::CCDirector::get()->getWinSize();
        this->setTitle("");
        m_buttonMenu->setContentSize(winSize);
        m_closeBtn->setPosition(20.f, winSize.height - 20.f);

        // Background for content
        m_contentBG = cocos2d::extension::CCScale9Sprite::create("GJ_square01.png");
        m_contentBG->setAnchorPoint({ 0, 1 });
        m_contentBG->setPosition(125.f, 270.f);
        m_contentBG->setColor({ 0, 0, 0 });
        m_contentBG->setOpacity(128);
        m_contentBG->setContentSize({ 345.f, 260.f });
        m_contentBG->setID("content-bg"_spr);
        m_mainLayer->addChild(m_contentBG);

        // Tab menu
        auto currentTab = config::get<int>("menu.current_tab", 0);
        currentTab = std::min(currentTab, static_cast<int>(tabs.size()) - 1);
        m_tabMenu = TabMenu::create(tabs, [this](int idx) {
            this->setActiveTab(idx);
        });
        m_tabMenu->setPosition(7.5f, 270.f);
        m_tabMenu->setActiveTab(currentTab);
        m_mainLayer->addChild(m_tabMenu);

        // Content view
        m_contentMenu = ContentView::create({ 345.f, 260.f }, tabs[currentTab]);
        m_contentMenu->setAnchorPoint({ 0, 1 });
        m_contentMenu->setPosition(125.f, 270.f);
        m_mainLayer->addChild(m_contentMenu, 1);

        return true;
    }

    Popup* Popup::create(Tabs const& tabs) {
        auto ret = new Popup;
        if (ret->initAnchored(480.f, 280.f, std::move(tabs))) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void Popup::setActiveTab(int idx) const {
        auto tabs = Engine::get()->getTabs();
        if (idx < 0 || idx >= tabs.size()) return;
        config::set("menu.current_tab", idx);
        m_contentMenu->setContent(tabs[idx]);
    }

    bool Popup::isAncestorOf(CCNode* node) const {
        while (node) {
            if (node == this) return true;
            node = node->getParent();
        }

        return false;
    }

    void Popup::onExit() {
        geode::Popup<Tabs const&>::onExit();
        CocosRenderer::get()->shutdown(true);
    }

}
