#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::cocos {

    /// @brief A container for tab buttons
    class TabMenu : public cocos2d::CCMenu {
    protected:
        std::vector<CCMenuItemSpriteExtra*> m_tabs;
        int m_activeTab = 0;

    public:
        static TabMenu* create(Tabs const& tabs, std::function<void(int)> const& callback);
        void setActiveTab(int idx);

    protected:
        bool init(Tabs const& tabs, std::function<void(int)> const& callback);
    };

    /// @brief Tab buttons for both activating and deactivating
    class TabButton : public cocos2d::CCNode {
    protected:
        cocos2d::extension::CCScale9Sprite* m_bgSprite;
        cocos2d::CCLabelBMFont* m_label;

    public:
        void setState(bool active);
        static TabButton* create(std::string name, cocos2d::CCSize size);

    protected:
        bool init(std::string name, cocos2d::CCSize size);
    };
}
