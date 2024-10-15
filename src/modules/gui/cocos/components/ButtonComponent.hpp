#pragma once
#include "BaseComponentNode.hpp"

namespace eclipse::gui::cocos {
    class ButtonComponentNode : public BaseComponentNode<ButtonComponentNode, CCMenuItemSpriteExtra, ButtonComponent, float> {
    public:
        void toggled(CCObject*) {
            m_component->triggerCallback();
        }

        bool init(float width) override {
            auto btnSprite = ButtonSprite::create(
                m_component->getTitle().c_str(), width * 0.75f, true,
                "bigFont.fnt", "GJ_button_01.png",
                28.f, 0.5
            );
            if (!btnSprite) return false;
            if (!CCMenuItemSpriteExtra::init(
                btnSprite, nullptr, this,
                menu_selector(ButtonComponentNode::toggled))
            ) return false;

            this->setContentSize({ width, 28.f });
            btnSprite->setPositionX(width / 2.f);
            this->m_scaleMultiplier = 1.25f;

            this->setID(fmt::format("button-{}"_spr, m_component->getId()));

            return true;
        }
    };
}