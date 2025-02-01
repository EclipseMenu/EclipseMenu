#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::cocos {
    class TranslatedLabel;

    /// @brief A container for tab buttons
    class TabMenu : public cocos2d::CCMenu {
    protected:
        CCMenuItemSpriteExtra* m_upArrow;
        CCMenuItemSpriteExtra* m_downArrow;
        std::vector<CCMenuItemSpriteExtra*> m_tabs;
        int m_activeTab = 0;
        int m_currentPage = 0;
        bool m_hasActivatedTab = false;

        ~TabMenu();

    public:
        static TabMenu* create(Tabs const& tabs, std::function<void(int)> const& callback);
        void setActiveTab(int idx);
        void regenTabs();

    protected:
        bool init(Tabs const& tabs, std::function<void(int)> const& callback);
    };

    /// @brief Tab buttons for both activating and deactivating
    class TabButton : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_bgSprite{};
        TranslatedLabel* m_label{};
        cocos2d::CCSprite* m_icon{};

    public:
        void setState(bool active) const;
        static TabButton* create(const std::string& name, const cocos2d::CCSize& size);

    protected:
        bool init(std::string name, cocos2d::CCSize size);
    };
}
