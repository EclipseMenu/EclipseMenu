#include "color-picker.hpp"

namespace eclipse::gui::cocos {
    bool ColorPicker::init(gui::Color const& original, bool useAlpha, std::function<void(gui::Color const&)> const& callback) {
        m_useAlpha = useAlpha;
        m_callback = callback;
        m_color = original;

        m_colorSprite = ColorChannelSprite::create();
        m_colorSprite->setScale(0.65F);
        m_colorSprite->setID("color-sprite");
        m_colorSprite->setColor(original.toCCColor3B());

        return CCMenuItemSpriteExtra::init(m_colorSprite, m_colorSprite, this, menu_selector(ColorPicker::onClicked));
    }

    void ColorPicker::updateColor(cocos2d::ccColor4B const& color) {
        m_color = gui::Color(color);
        m_colorSprite->setColor(m_color.toCCColor3B());
        m_callback(m_color);
    }

    void ColorPicker::onClicked(CCObject*) {
        if (m_useAlpha) {
            m_popup = ColorPopup::create(static_cast<cocos2d::ccColor4B>(m_color));
        } else {
            m_popup = ColorPopup::create(m_color.toCCColor3B());
        }

        m_popup->setDelegate(this);
        m_popup->show();

        if (auto cocos = CocosRenderer::get()) {
            cocos->registerModal(m_popup);
        }
    }
}
