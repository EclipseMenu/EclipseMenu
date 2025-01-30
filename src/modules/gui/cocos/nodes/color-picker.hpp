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

        void setColor(gui::Color const& color) {
            m_color = color;
            m_colorSprite->setColor(color.toCCColor3B());
        }

    protected:
        bool init(gui::Color const& original, bool useAlpha, std::function<void(gui::Color const&)> const& callback);
        void onClicked(CCObject*);
        void updateColor(cocos2d::ccColor4B const& color) override;

    protected:
        cocos2d::CCSprite* m_colorSprite = nullptr;
        std::function<void(gui::Color const&)> m_callback;
        gui::Color m_color;
        bool m_useAlpha = false;
    };
}
