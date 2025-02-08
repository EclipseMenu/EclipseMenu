#pragma once
#include <memory>
#include <modules/gui/color.hpp>
#include <modules/gui/cocos/cocos.hpp>

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
        geode::ColorPickPopup* m_popup = nullptr;
    };

    class ColorPopup : public geode::ColorPickPopup {
        ~ColorPopup() override {
            if (auto cocos = CocosRenderer::get())
                cocos->unregisterModal(this);
        }

    public:
        static ColorPopup* create(cocos2d::ccColor4B const& color, bool isRGBA) {
            auto ret = new ColorPopup();
            if (ret->initAnchored(400.f, (isRGBA ? 290.f : 240.f), color, isRGBA)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        static ColorPopup* create(cocos2d::ccColor3B const& color) {
            return ColorPopup::create(geode::cocos::to4B(color), true);
        }

        static ColorPopup* create(cocos2d::ccColor4B const& color) {
            return ColorPopup::create(color, true);
        }
    };
}
