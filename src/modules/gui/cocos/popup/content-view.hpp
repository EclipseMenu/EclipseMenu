#pragma once
#include <modules/gui/cocos/popup/scroll-layer.hpp>

namespace eclipse::gui {
    class MenuTab;
}

namespace eclipse::gui::cocos {

    /// @brief A container that displays the current tab components.
    class ContentView : public cocos2d::CCNode {
    protected:
        //geode::ScrollLayer* m_contentLayer = nullptr;
        ScrollLayer* m_contentLayer = nullptr;

    public:
        static ContentView* create(cocos2d::CCSize const &size, const std::shared_ptr<MenuTab>& tab);
        void setContent(const std::shared_ptr<MenuTab>& tab);

    protected:
        bool init(cocos2d::CCSize const &size, const std::shared_ptr<MenuTab> &tab);
        void loadContent(const std::shared_ptr<MenuTab>& tab) const;
    };

}
