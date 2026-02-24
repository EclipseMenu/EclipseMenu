#pragma once
#include <modules/gui/gui.hpp>

namespace eclipse::gui::cocos {
    class TabMenu;
    class ContentView;

    class Popup : public geode::Popup {
    protected:
        bool init(Tabs const& tabs);

    public:
        static Popup* create(Tabs const& tabs);

        ~Popup() override;

        void setActiveTab(int idx) const;
        void refreshPage() const;

        bool isAncestorOf(CCNode* node) const;

    private:
        TabMenu* m_tabMenu = nullptr;

        geode::NineSlice* m_contentBG = nullptr;
        ContentView* m_contentMenu = nullptr;
    };
}
