#pragma once
#include <functional.hpp>

namespace eclipse::labels {
    struct LabelSettings;
}

namespace eclipse::hacks::Labels {
    class SmartLabel;
}

namespace eclipse::gui::cocos {
    enum class CallbackEvent { Update, Export };

    class LabelSettingsPopup : public geode::Popup<labels::LabelSettings*, StdFunction<void(CallbackEvent)>&&> {
    protected:
        bool setup(labels::LabelSettings* settings, StdFunction<void(CallbackEvent)>&& callback) override;

        CCLayer* createSettingsTab();
        CCLayer* createTextTab();
        CCLayer* createEventsTab();
        CCLayer* createPreviewTab();

        void updatePreview(float dt);

    public:
        static LabelSettingsPopup* create(labels::LabelSettings* settings, StdFunction<void(CallbackEvent)>&& callback);
        void selectTab(size_t index);

        ~LabelSettingsPopup() override;

    private:
        labels::LabelSettings* m_settings{};
        StdFunction<void(CallbackEvent)> m_callback;
        std::vector<class PopupTab*> m_tabs;
        CCLayer* m_currentTab = nullptr;
        cocos2d::extension::CCScale9Sprite* m_contentBG = nullptr;
        hacks::Labels::SmartLabel* m_previewLabel = nullptr;
        std::array<cocos2d::CCSprite*, 9> m_alignButtons{};
        std::array<cocos2d::CCSprite*, 3> m_fontAlignButtons{};
    };
}
