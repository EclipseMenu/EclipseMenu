#pragma once
#include <functional.hpp>

namespace eclipse::gui {
    class LabelSettingsComponent;
}

namespace eclipse::labels {
    struct LabelEvent;
    struct LabelSettings;
}

namespace eclipse::hacks::Labels {
    class SmartLabel;
}

namespace eclipse::gui::cocos {
    class LabelSettingsPopup : public geode::Popup<std::shared_ptr<LabelSettingsComponent>> {
    protected:
        bool setup(std::shared_ptr<LabelSettingsComponent> component) override;

        CCLayer* createSettingsTab();
        CCLayer* createTextTab();
        CCLayer* createEventsTab();
        CCLayer* createPreviewTab();

        void updatePreview(float dt);

        CCNode* createEventCard(labels::LabelEvent& event, size_t index);

    public:
        static LabelSettingsPopup* create(std::shared_ptr<LabelSettingsComponent> component);
        void selectTab(size_t index);
        void onExport(CCObject*);
        void onCreateEvent(CCObject*);

        ~LabelSettingsPopup() override;

    private:
        std::shared_ptr<LabelSettingsComponent> m_component{};
        std::array<class PopupTab*, 4> m_tabs{};
        CCLayer* m_currentTab = nullptr;
        cocos2d::extension::CCScale9Sprite* m_contentBG = nullptr;
        hacks::Labels::SmartLabel* m_previewLabel = nullptr;
        CCContentLayer* m_eventsContentLayer = nullptr;
        std::array<cocos2d::CCSprite*, 9> m_alignButtons{};
        std::array<cocos2d::CCSprite*, 3> m_fontAlignButtons{};
    };
}
