#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/components/button.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::cocos {
    class ButtonComponentNode : public BaseComponentNode<ButtonComponentNode, CCMenuItemSpriteExtra, ButtonComponent, float> {
    public:
        void toggled(CCObject*) {
            m_component->triggerCallback();
        }

        bool init(float width) override {
            const auto tm = ThemeManager::get();

            auto label = TranslatedLabel::create(m_component->getTitle());
            auto bg = cocos2d::extension::CCScale9Sprite::create("square.png", { 0.0f, 0.0f, 80.0f, 80.0f });
            if (!bg || !label) return false;

            width *= 0.9f;

            bg->setContentSize({width, 28.f});
            bg->setColor(tm->getButtonBackgroundColor().toCCColor3B());
            label->setColor(tm->getButtonActivatedForeground().toCCColor3B());
            // label->setScale(0.5F);
            bg->addChildAtPosition(label, geode::Anchor::Center);
            if (!CCMenuItemSpriteExtra::init(
                bg, nullptr, this,
                menu_selector(ButtonComponentNode::toggled))
            ) return false;

            this->setContentSize({ width, 28.f });
            bg->setPositionX(width / 2.f);
            this->m_scaleMultiplier = 1.1f;

            this->setID(fmt::format("button-{}"_spr, m_component->getId()));

            return true;
        }
    };
}
