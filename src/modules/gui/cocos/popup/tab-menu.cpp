#include "tab-menu.hpp"

#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::cocos {
    TabMenu* TabMenu::create(Tabs const& tabs, std::function<void(int)> const& callback) {
        auto ret = new TabMenu();
        if (ret->init(tabs, callback)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    TabButton* TabButton::create(const std::string& name, const cocos2d::CCSize& size) {
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

    bool TabMenu::init(Tabs const& tabs, std::function<void(int)> const& callback) {
        if (!CCMenu::init()) return false;
        this->setID("tab-menu"_spr);
        //std::vector<std::shared_ptr<MenuTab>> newTabs = tabs;
        //newTabs.pop_back();

        int i = 0;
        constexpr float width = 120.f;
        constexpr float height = 28.f;
        for (auto const& tab : tabs) {
            auto tabName = tab->getTitle();
            auto tabSpr = TabButton::create(tabName, {width, height});
            auto tabButton = geode::cocos::CCMenuItemExt::createSpriteExtra(
                tabSpr,
                [this, callback](auto caller) {
                    auto tag = caller->getTag();
                    this->setActiveTab(tag);
                    callback(tag);
                    // this is so dumb
                    m_hasActivatedTab = true;
                    if (auto delegate = geode::cast::typeinfo_cast<CCTouchDelegate*>(this)) {
                        if (auto handler = cocos2d::CCTouchDispatcher::get()->findHandler(delegate)) {
                            if (auto dispatcher = cocos2d::CCTouchDispatcher::get()) {
                                //dispatcher->setPriority(handler->m_nPriority - 1, delegate);
                                dispatcher->setPriority(-503, delegate);
                            }
                        }
                    }
                }
            );
            tabButton->setTag(i++);
            this->addChild(tabButton);
            m_tabs.push_back(tabButton);
        }

        // setup layout
        auto layout = geode::AxisLayout::create(geode::Axis::Column)
                      //->setAxisReverse(true)
                      ->setAutoScale(true)
                      //->setAutoScale(false)
                      ->setAxisReverse(true)
                      ->setGrowCrossAxis(false)
                      ->setCrossAxisOverflow(true)
                      ->setGap(0.5f)
                      ->setAxisAlignment(geode::AxisAlignment::End)
                      ->setCrossAxisAlignment(geode::AxisAlignment::Start)
                      ->setCrossAxisLineAlignment(geode::AxisAlignment::Start);
        this->setAnchorPoint({0.5f, 0.5f});
        this->setContentHeight(260.f);
        this->setLayout(layout, true);

        this->setActiveTab(0);

        auto upButton = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");
        upButton->setFlipY(true);
        auto downButton = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");

        if (auto delegate = geode::cast::typeinfo_cast<CCTouchDelegate*>(this)) {
            if (auto handler = cocos2d::CCTouchDispatcher::get()->findHandler(delegate)) {
                if (auto dispatcher = cocos2d::CCTouchDispatcher::get()) {
                    //dispatcher->setPriority(handler->m_nPriority - 1, delegate);
                    dispatcher->setPriority(-503, delegate);
                }
            }
        }

        return true;
    }

    bool TabButton::init(std::string name, cocos2d::CCSize size) {
        if (!CCNode::init()) return false;
        this->setScale(0.9F);
        this->setID(fmt::format("tab-button-{}"_spr, name));
        this->setContentSize(size);

        m_label = TranslatedLabel::create(name);
        m_label->limitLabelWidth(100, 1.f, .2f);
        m_bgSprite = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0.0f, 0.0f, 80.0f, 80.0f});
        m_bgSprite->setContentSize({size.width, size.height + 8.F}); // minimum 36
        m_bgSprite->setScaleY(.75F);
        this->addChildAtPosition(m_bgSprite, geode::Anchor::Center);
        this->addChildAtPosition(m_label, geode::Anchor::Center);
        return true;
    }

    void TabButton::setState(bool active) {
        // this definitely won't be a problem in the future
        if (m_bgSprite == nullptr || m_label == nullptr) return;
        const auto tm = ThemeManager::get();
        auto colorLbl = (!active) ? tm->getButtonForegroundColor() : tm->getButtonActivatedForeground();
        auto colorBG = (!active) ? tm->getButtonBackgroundColor() : tm->getButtonActivatedBackground();
        m_bgSprite->setColor(colorBG.toCCColor3B());
        m_label->setColor(colorLbl.toCCColor3B());
    }
}
