#pragma once

namespace eclipse::gui {
    class MenuTab;
}

namespace eclipse::gui::cocos {
    class ScrollLayer;

    /// @brief A container that displays the current tab components.
    class ContentView : public cocos2d::CCNode {
    protected:
        //geode::ScrollLayer* m_contentLayer = nullptr;
        ScrollLayer* m_contentLayer = nullptr;

    public:
        static ContentView* create(cocos2d::CCSize const& size, std::shared_ptr<MenuTab> const& tab);
        void setContent(std::shared_ptr<MenuTab> const& tab, bool resetScroll = true) const;

    protected:
        bool init(cocos2d::CCSize const& size, std::shared_ptr<MenuTab> const& tab);
        void loadContent(std::shared_ptr<MenuTab> const& tab) const;
    };
}
