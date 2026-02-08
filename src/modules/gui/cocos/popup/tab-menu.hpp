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
        Function<void(int)> m_callback;
        int m_activeTab = 0;
        int m_currentPage = 0;
        bool m_hasActivatedTab = false;

        ~TabMenu() override;

        void onPageButton(CCObject* sender);
        void onArrowButton(CCObject* sender);

    public:
        static TabMenu* create(Tabs const& tabs, Function<void(int)>&& callback);
        void setActiveTab(int idx);
        void regenTabs();

    protected:
        bool init(Tabs const& tabs, Function<void(int)>&& callback);
    };

    /// @brief Tab buttons for both activating and deactivating
    class TabButton : public cocos2d::CCNode {
    protected:
        geode::NineSlice* m_bgSprite{};
        TranslatedLabel* m_label{};
        cocos2d::CCSprite* m_icon{};

    public:
        void setState(bool active) const;
        static TabButton* create(std::string_view name, cocos2d::CCSize const& size);

    protected:
        bool init(std::string_view name, cocos2d::CCSize size);
    };
}
