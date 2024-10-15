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

}