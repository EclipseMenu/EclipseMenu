#pragma once
#include "BaseComponentNode.hpp"

#include <Geode/ui/ColorPickPopup.hpp>
#include <modules/gui/cocos/nodes/color-picker.hpp>
#include <modules/gui/components/color.hpp>

namespace eclipse::gui::cocos {
    class ColorComponentNode : public BaseComponentNode<ColorComponentNode, cocos2d::CCMenu, ColorComponent, float> {
    protected:
        ColorPicker* m_colorBtn = nullptr;
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;

    public:
        bool init(float width) {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("color-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            m_colorBtn = ColorPicker::create(m_component->getValue(), m_component->hasOpacity(), [this](auto color) {
                m_component->setValue(color);
                m_component->triggerCallback(color);
            });

            this->addChildAtPosition(m_colorBtn, geode::Anchor::Left, { 15.f, 0.f });

            auto labelSize = width - 35.f;
            auto offset = 0.f;

            if (!m_component->getDescription().empty()) {
                m_infoButton = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName("info.png"_spr, 0.35f, [this](auto) {
                    this->openDescriptionPopup();
                });
                m_infoButton->setAnchorPoint({ 0.5, 0.5f });
                m_infoButton->setColor(ThemeManager::get()->getCheckboxCheckmarkColor().toCCColor3B());
                this->addChildAtPosition(m_infoButton, geode::Anchor::Right, { offset - 10.f, 0.f });
                labelSize -= 15.f;
            }

            m_label = TranslatedLabel::create(m_component->getTitle());
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 30.f, 0.f });

            return true;
        }
    };
}
