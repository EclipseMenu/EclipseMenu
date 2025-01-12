#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::cocos {
    class TabMenu;
    class ContentView;

    class Popup : public geode::Popup<Tabs const&> {
    protected:
        bool setup(Tabs const& tabs) override;
        void onExit() override;

    public:
        static Popup* create(Tabs const& tabs);

        void setActiveTab(int idx) const;
        void refreshPage() const;

        bool isAncestorOf(CCNode* node) const;

    private:
        TabMenu* m_tabMenu = nullptr;

        cocos2d::extension::CCScale9Sprite* m_contentBG = nullptr;
        ContentView* m_contentMenu = nullptr;
    };
}
