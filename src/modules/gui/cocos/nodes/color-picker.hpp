#pragma once
#include <modules/gui/color.hpp>

namespace eclipse::gui::cocos {
    class ColorPicker : public CCMenuItemSpriteExtra, geode::ColorPickPopupDelegate {
    public:
        static ColorPicker* create(gui::Color const& original, bool useAlpha, std::function<void(gui::Color const&)> const& callback) {
            auto ret = new ColorPicker();
            if (ret->init(original, useAlpha, callback)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    protected:
        bool init(gui::Color const& original, bool useAlpha, std::function<void(gui::Color const&)> const& callback);
        void updateColor(cocos2d::ccColor4B const& color) override;
        void onClicked(CCObject*);

    protected:
        cocos2d::CCSprite* m_colorSprite = nullptr;
        std::function<void(gui::Color const&)> m_callback;
        gui::Color m_color;
        bool m_useAlpha = false;
    };
}
