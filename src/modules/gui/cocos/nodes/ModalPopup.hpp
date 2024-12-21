#pragma once
#include <modules/gui/popup.hpp>

namespace eclipse::gui::cocos {

    class ModalPopup : public geode::Popup<eclipse::Popup const&> {
    protected:
        bool setup(eclipse::Popup const& settings) override;
        void onExit() override;
        cocos2d::CCNode* createButtonSprite(std::string const& text) const;
        void keyBackClicked() override;

        bool m_cancelable = true;
        eclipse::Popup m_settings;
    public:
        static ModalPopup* create(eclipse::Popup const& settings);
    };
}
