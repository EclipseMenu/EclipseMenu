#include "tab-menu.hpp"

#include "utils.hpp"

#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
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

    TabMenu::~TabMenu() {}

    void TabMenu::setActiveTab(int idx) {
        if (idx < 0 || idx >= m_tabs.size()) return;

        m_activeTab = idx;
        if (m_activeTab > 8) {
            m_currentPage = std::max(0, m_activeTab - 8);
        }
        for (int i = 0; i < m_tabs.size(); ++i) {
            bool active = i == idx;
            auto tab = m_tabs[i];
            tab->setEnabled(!active);
            auto spr = static_cast<TabButton*>(tab->getChildren()->objectAtIndex(0));
            spr->setState(active);
        }
    }

    void TabMenu::regenTabs() {
        for (auto const& tab : m_tabs) {
            tab->setContentHeight(0.f);
            tab->setVisible(false);
        }
        constexpr int amount = 9;
        auto newTabs = utils::gradualPaginate<CCMenuItemSpriteExtra*>(m_tabs, amount, m_currentPage);
        m_upArrow->setVisible(m_currentPage != 0 && m_tabs.size() > 10);
        m_downArrow->setVisible(m_tabs.size() > 10 && m_currentPage < (m_tabs.size() - amount));
        for (auto const& tab : newTabs) {
            tab->setContentHeight(28.f);
            tab->setVisible(true);
        }
        this->updateLayout();
    }
    bool TabMenu::init(Tabs const& tabs, std::function<void(int)> const& callback) {
        if (!CCMenu::init()) return false;
        this->setID("tab-menu"_spr);

        auto upSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");
        upSpr->setScale(1.5F);
        upSpr->setFlipY(true);
        m_upArrow = geode::cocos::CCMenuItemExt::createSpriteExtra(upSpr, [this](auto caller){
            m_currentPage--;
            regenTabs();
        });
        this->addChild(m_upArrow);

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
                }
            );
            tabButton->setTag(i++);
            this->addChild(tabButton);
            tabButton->setVisible(false);
            m_tabs.push_back(tabButton);
        }
        auto downSpr = cocos2d::CCSprite::createWithSpriteFrameName("GJ_chatBtn_01_001.png");
        downSpr->setScale(1.5F);
        m_downArrow = geode::cocos::CCMenuItemExt::createSpriteExtra(downSpr, [this](auto caller){
            m_currentPage++;
            regenTabs();
        });
        this->addChild(m_downArrow);

        regenTabs();

        // setup layout
        auto layout = geode::AxisLayout::create(geode::Axis::Column)
                      //->setAutoScale(true)
                      ->setAutoScale(false)
                      ->setAxisReverse(true)
                      ->setGrowCrossAxis(false)
                      ->setCrossAxisOverflow(true)
                      ->setGap(0.5f)
                      ->setAxisAlignment(geode::AxisAlignment::End)
                      ->setCrossAxisAlignment(geode::AxisAlignment::Start)
                      ->setCrossAxisLineAlignment(geode::AxisAlignment::Center);
        this->setAnchorPoint({0.5f, 0.5f});
        this->setContentHeight(260.f);
        this->setLayout(layout, true);

        this->setActiveTab(0);

        return true;
    }

    inline TranslatedLabel* mapWithIcon(std::string_view name) {
        auto label = TranslatedLabel::create(name);

        static const std::unordered_map<std::string_view, std::string_view> icons = {
            {"tab.global", "🌐 "},
            {"tab.level", "⭐ "},
            {"tab.bypass", "🔓 "},
            {"tab.player", "🎮 "},
            {"tab.bot", "🤖 "},
            {"tab.creator", "🛠️ "},
            {"tab.labels", "🏷️ "},
            {"tab.shortcuts", "🔗 "},
            {"tab.keybinds", "⌨️ "},
            {"tab.interface", "⚙️ "},
            {"tab.recorder", "📹 "},
            {"BetterInfo", "🇮 "},
        };

        static const Label::EmojiMap emojis = {
            {U"🌐", "tab_global.png"_spr},
            {U"⭐", "tab_level.png"_spr},
            {U"🔓", "tab_bypass.png"_spr},
            {U"🎮", "tab_player.png"_spr},
            {U"🤖", "tab_bot.png"_spr},
            {U"🛠️", "tab_creator.png"_spr},
            {U"🏷️", "tab_labels.png"_spr},
            {U"🔗", "tab_shortcuts.png"_spr},
            {U"⌨️", "tab_keybinds.png"_spr},
            {U"⚙️", "tab_interface.png"_spr},
            {U"📹", "tab_recorder.png"_spr},
            {U"🇮", "tab_betterinfo.png"_spr},
        };

        std::string_view icon = "";
        if (auto it = icons.find(name); it != icons.end()) {
            icon = it->second;
        }

        label->enableEmojis("UISheet.png"_spr, &emojis);
        label->enableEmojiColors(true);
        label->setString(fmt::format("{}{}", icon, label->getString()));
        label->limitLabelWidth(100, 1.f, .2f);
        return label;
    }

    bool TabButton::init(std::string name, cocos2d::CCSize size) {
        if (!CCNode::init()) return false;
        this->setScale(0.9F);
        this->setID(fmt::format("tab-button-{}"_spr, name));
        this->setContentSize(size);

        m_label = mapWithIcon(name);
        m_bgSprite = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", {0.0f, 0.0f, 80.0f, 80.0f});
        m_bgSprite->setContentSize({size.width, size.height + 8.F}); // minimum 36
        m_bgSprite->setScaleY(.75F);
        this->addChildAtPosition(m_bgSprite, geode::Anchor::Center);
        this->addChildAtPosition(m_label, geode::Anchor::Center);
        return true;
    }

    void TabButton::setState(bool active) const {
        // this definitely won't be a problem in the future
        if (m_bgSprite == nullptr || m_label == nullptr) return;
        const auto tm = ThemeManager::get();
        auto colorLbl = (!active) ? tm->getButtonForegroundColor() : tm->getButtonActivatedForeground();
        auto colorBG = (!active) ? tm->getButtonBackgroundColor() : tm->getButtonActivatedBackground();
        m_bgSprite->setColor(colorBG.toCCColor3B());
        m_label->setColor(colorLbl.toCCColor3B());
    }
}
