#pragma once

namespace eclipse::gui {
    class MenuTab;
}

namespace eclipse::gui::cocos {
    class ContentView;

    /// @brief A popup with a ContentView for displaying extra hack options.
    class OptionsPopup : public geode::Popup {
    protected:
        bool init(MenuTab const& tab);
        void onExit() override;

    public:
        static OptionsPopup* create(MenuTab const& tab);

    protected:
        cocos2d::extension::CCScale9Sprite* m_contentBG = nullptr;
        ContentView* m_contentView = nullptr;
    };
}
