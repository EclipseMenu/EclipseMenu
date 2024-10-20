#include "tab-menu.hpp"
#include <modules/gui/theming/manager.hpp>

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

    TabButton* TabButton::create(std::string name, cocos2d::CCSize size) {
        auto ret = new TabButton();
        if (ret->init(name, size)) {
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
            auto spr = static_cast<TabButton*>(tab->getChildren()->objectAtIndex(0));
            spr->setState(active);
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
            auto tabSpr = TabButton::create(tabName, { width, height });
            auto tabButton = geode::cocos::CCMenuItemExt::createSpriteExtra(
                //ButtonSprite::create(tabName.c_str(), width, true, "bigFont.fnt", "GJ_button_01.png", height, 0.5f),
                tabSpr,
                [this, callback](auto caller) {
                    auto tag = caller->getTag();
                    this->setActiveTab(tag);
                    callback(tag);
                }
            );
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

    bool TabButton::init(std::string name, cocos2d::CCSize size) {
        if (!CCNode::init()) return false;
        this->setID(fmt::format("tab-button-{}"_spr, name));
        size.width += 15.F; // 135
        this->setContentSize(size);

        m_label = cocos2d::CCLabelBMFont::create(name.c_str(), "bigFont.fnt");
        m_label->limitLabelWidth(100, 0.5F, .2F);
        m_bgSprite = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
        m_bgSprite->setContentSize({size.width, size.height + 8.F}); // minimum 36
        m_bgSprite->setScaleY(.75F);
        this->addChildAtPosition(m_bgSprite, cocos2d::Anchor::Center);
        this->addChildAtPosition(m_label, cocos2d::Anchor::Center);
        return true;
    }

    void TabButton::setState(bool active) {
        // this definitely wont be a problem in the future
        if (m_bgSprite == nullptr || m_label == nullptr) return;
        const auto tm = ThemeManager::get();
        auto colorLbl = (!active) ? tm->getButtonForegroundColor() : tm->getButtonActivatedForeground();
        auto colorBG = (!active) ? tm->getButtonBackgroundColor() : tm->getButtonActivatedBackground();
        m_bgSprite->setColor(colorBG.toCCColor3B());
        m_label->setColor(colorLbl.toCCColor3B());
    }
}
