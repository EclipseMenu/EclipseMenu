#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/components/combo.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::cocos {
    class ComboComponentNode : public BaseComponentNode<ComboComponentNode, cocos2d::CCMenu, ComboComponent, float> {
    protected:
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;
        cocos2d::extension::CCScale9Sprite* m_background = nullptr;
        TranslatedLabel* m_valueLabel = nullptr;

    public:
        void updateLabel() const {
            int index = m_component->getValue();
            if (index < 0 || index >= m_component->getItems().size()) {
                m_valueLabel->setString("");
                return;
            }
            m_valueLabel->setString(m_component->getItems()[index]);
            m_valueLabel->limitLabelWidth(200.f, 2.f, 0.25f);
        }

        void scroll(CCObject* sender) {
            int tag = sender->getTag();
            int currentIndex = m_component->getValue();
            int value = currentIndex + tag;
            if (value < 0) value = std::max<int>(m_component->getItems().size() - 1, 0);
            if (value >= m_component->getItems().size()) value = 0;
            m_component->setValue(value);
            m_component->triggerCallback(value);
            updateLabel();
        }

        bool init(float width) override {
            if (!CCMenu::init()) return false;
            const auto tm = ThemeManager::get();

            this->setID(fmt::format("combo-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            auto labelSize = (width * 0.6f) - 35.f;

            if (!m_component->getDescription().empty()) {
                m_infoButton = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName("GJ_infoIcon_001.png", 0.5f, [this](auto) {
                    this->openDescriptionPopup();
                });
                m_infoButton->setAnchorPoint({ 0.5, 0.5f });
                this->addChildAtPosition(m_infoButton, geode::Anchor::Right, { -10.f, 0.f });
                labelSize -= 15.f;
            }

            m_label = TranslatedLabel::create(m_component->getTitle());
            m_label->setColor(tm->getForegroundColor().toCCColor3B());
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 15.f, 0.f });

            m_background = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
            m_background->setID("background");
            m_background->setAnchorPoint({ 0.5f, 0.5f });
            m_background->setScale(0.3f);
            m_background->setContentSize({365.f, 80.f});
            m_background->setColor(tm->getFrameBackground().toCCColor3B());
            m_background->setZOrder(-1);
            this->addChildAtPosition(m_background, geode::Anchor::Right, { -70.f, 0.f });

            auto spr = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
            spr->setScale(0.6f);
            auto arrowBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ComboComponentNode::scroll));
            arrowBtn->setTag(1);
            this->addChildAtPosition(arrowBtn, geode::Anchor::Right, { -25.f, 0.f });

            auto arrowBtn2 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ComboComponentNode::scroll));
            arrowBtn2->setRotation(180.f);
            arrowBtn2->setTag(-1);
            this->addChildAtPosition(arrowBtn2, geode::Anchor::Right, { -115.f, 0.f });

            int index = m_component->getValue();
            if (index < 0 || index >= m_component->getItems().size()) {
                m_valueLabel = TranslatedLabel::createRaw("");
            } else {
                m_valueLabel = TranslatedLabel::createRaw(m_component->getItems().at(m_component->getValue()));
            }
            m_background->addChildAtPosition(m_valueLabel, geode::Anchor::Center, { 0.f, 0.f });
            updateLabel();

            return true;
        }
    };
}
