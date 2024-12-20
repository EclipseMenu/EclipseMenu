#pragma once
#include "BaseComponentNode.hpp"
#include "Geode/ui/ColorPickPopup.hpp"
#include <modules/gui/cocos/popup/options-popup.hpp>

namespace eclipse::gui::cocos {
    class ColorComponentNode : public BaseComponentNode<ColorComponentNode, cocos2d::CCMenu, ColorComponent, float>, public geode::ColorPickPopupDelegate {
    protected:
        ColorChannelSprite* m_colorSprite = nullptr;
        CCMenuItemSpriteExtra* m_colorBtn = nullptr;
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;

        void updateColor(cocos2d::ccColor4B const& color) override {
            auto colorValue = Color(color);
            m_colorSprite->setColor(geode::cocos::to3B(color));
            m_component->setValue(colorValue);
            m_component->triggerCallback(colorValue);
        }

    public:
        bool init(float width) override {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("color-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            // definitely not copied from geode
            m_colorSprite = ColorChannelSprite::create();
            m_colorSprite->setScale(0.65F);
            m_colorBtn = geode::cocos::CCMenuItemExt::createSpriteExtra(m_colorSprite, [this](auto) {
                geode::ColorPickPopup* popup;
                if (m_component->hasOpacity()) {
                    popup = geode::ColorPickPopup::create(static_cast<cocos2d::ccColor4B>(m_component->getValue()));
                } else {
                    popup = geode::ColorPickPopup::create(m_component->getValue().toCCColor3B());
                }
                popup->setDelegate(this);
                popup->show();
            });

            m_colorBtn->setAnchorPoint({ 0.5, 0.5f });
            m_colorSprite->setColor(m_component->getValue().toCCColor3B());
            this->addChildAtPosition(m_colorBtn, geode::Anchor::Left, { 15.f, 0.f });

            auto labelSize = width - 35.f;
            auto offset = 0.f;

            if (!m_component->getDescription().empty()) {
                m_infoButton = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName("GJ_infoIcon_001.png", 0.5f, [this](auto) {
                    FLAlertLayer::create(
                        m_component->getTitle().c_str(), m_component->getDescription().c_str(), "OK"
                    )->show();
                });
                m_infoButton->setAnchorPoint({ 0.5, 0.5f });
                this->addChildAtPosition(m_infoButton, geode::Anchor::Right, { offset - 10.f, 0.f });
                labelSize -= 15.f;
            }

            m_label = TranslatedLabel::create(m_component->getTitle());
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 0.6f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 30.f, 0.f });

            return true;
        }
    };
}
