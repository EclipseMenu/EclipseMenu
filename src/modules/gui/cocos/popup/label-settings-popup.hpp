#pragma once

namespace eclipse::labels {
    struct LabelSettings;
}

namespace eclipse::hacks::Labels {
    class SmartLabel;
}

namespace eclipse::gui::cocos {
    enum class CallbackEvent { Update, Export };

    class LabelSettingsPopup : public geode::Popup<labels::LabelSettings*, std::function<void(CallbackEvent)> const&> {
    protected:
        bool setup(labels::LabelSettings* settings, std::function<void(CallbackEvent)> const& callback) override;
        void onExit() override;

        CCLayer* createSettingsTab();
        CCLayer* createTextTab() const;
        CCLayer* createEventsTab() const;
        CCLayer* createPreviewTab();

        void updatePreview(float dt);

    public:
        static LabelSettingsPopup* create(labels::LabelSettings* settings, std::function<void(CallbackEvent)> const& callback);
        void selectTab(size_t index);

    private:
        labels::LabelSettings* m_settings{};
        std::function<void(CallbackEvent)> m_callback;
        std::vector<class PopupTab*> m_tabs;
        CCLayer* m_currentTab = nullptr;
        cocos2d::extension::CCScale9Sprite* m_contentBG = nullptr;
        hacks::Labels::SmartLabel* m_previewLabel = nullptr;
        std::array<cocos2d::CCSprite*, 9> m_alignButtons{};
        std::array<cocos2d::CCSprite*, 3> m_fontAlignButtons{};
    };
}
