#include "tab-menu.hpp"

namespace eclipse::gui::cocos {
    TabMenu* TabMenu::create(Tabs const &tabs, std::function<void(int)> const& callback) {
        auto ret = new TabMenu();
        if (ret->init(tabs, callback)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void TabMenu::setActiveTab(int idx) {
        if (idx < 0 || idx >= m_tabs.size()) return;

        m_activeTab = idx;
        for (int i = 0; i < m_tabs.size(); ++i) {
            bool active = i == idx;
            auto tab = m_tabs[i];
            tab->setEnabled(!active);
            auto spr = static_cast<ButtonSprite*>(tab->getChildren()->objectAtIndex(0));
            spr->updateBGImage(active ? "GJ_button_02.png" : "GJ_button_01.png");
        }
    }

    bool TabMenu::init(Tabs const &tabs, std::function<void(int)> const& callback) {
        if (!CCMenu::init()) return false;
        this->setID("tab-menu"_spr);

        int i = 0;
        constexpr float width = 120.f;
        constexpr float height = 28.f;
        for (auto const& tab : tabs) {
            auto tabName = tab->getTitle();
            auto tabButton = geode::cocos::CCMenuItemExt::createSpriteExtra(
                ButtonSprite::create(tabName.c_str(), width, true, "bigFont.fnt", "GJ_button_01.png", height, 0.5f),
                [this, callback](auto caller) {
                    auto tag = caller->getTag();
                    this->setActiveTab(tag);
                    callback(tag);
                });
            tabButton->setTag(i++);
            this->addChild(tabButton);
            m_tabs.push_back(tabButton);
        }

        // setup layout
        auto layout = cocos2d::AxisLayout::create(cocos2d::Axis::Column);
        layout->setAxisReverse(true);
        layout->setAutoScale(true);
        layout->setGrowCrossAxis(false);
        layout->setCrossAxisOverflow(true);
        layout->setGap(0.5f);
        layout->setAxisAlignment(cocos2d::AxisAlignment::End);
        layout->setCrossAxisAlignment(cocos2d::AxisAlignment::Start);
        layout->setCrossAxisLineAlignment(cocos2d::AxisAlignment::Start);
        this->setAnchorPoint({0.f, 1.f});
        this->setContentHeight(260.f);
        this->setLayout(layout, true);

        this->setActiveTab(0);

        return true;
    }
}
