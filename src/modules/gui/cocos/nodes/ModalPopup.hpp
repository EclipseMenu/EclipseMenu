#pragma once
#include <modules/gui/popup.hpp>

namespace eclipse::gui::cocos {

    class ModalPopup : public geode::Popup {
    protected:
        bool init(eclipse::Popup settings);
        void onExit() override;
        cocos2d::CCNode* createButtonSprite(geode::ZStringView text) const;
        void keyBackClicked() override;

        void onBtn1(cocos2d::CCObject* sender);
        void onBtn2(cocos2d::CCObject* sender);

        bool m_cancelable = true;
        eclipse::Popup m_settings;
    public:
        static ModalPopup* create(eclipse::Popup&& settings);
    };
}
