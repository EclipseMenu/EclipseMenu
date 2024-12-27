#pragma once
#include "BaseComponentNode.hpp"
#include <modules/gui/cocos/popup/options-popup.hpp>

namespace eclipse::gui::cocos {
    class ComboComponentNode : public BaseComponentNode<ComboComponentNode, cocos2d::CCMenu, ComboComponent, float> {
    protected:
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;
        cocos2d::extension::CCScale9Sprite* m_background = nullptr;
        cocos2d::CCLabelBMFont* m_valueLabel = nullptr;

    public:
        void updateLabel() {
            m_valueLabel->setString(m_component->getItems()[m_component->getValue()].c_str());
            m_valueLabel->limitLabelWidth(200.f, 1.f, 0.25f);
        }

        void right(CCObject* sender) {
            int value = m_component->getValue() == m_component->getItems().size() - 1 ? 0 : m_component->getValue() + 1;
            m_component->setValue(value);
            updateLabel();
        }

        void left(CCObject* sender) {
            int value = m_component->getValue() == 0 ? m_component->getItems().size() - 1 : m_component->getValue() - 1;
            m_component->setValue(value);
            updateLabel();
        }

        bool init(float width) override {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("toggle-{}"_spr, m_component->getId()));
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
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 15.f, 0.f });

            m_background = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
            m_background->setID("background");
            m_background->setAnchorPoint({ 0.5f, 0.5f });
            m_background->setScale(0.3f);
            m_background->setContentSize({365.f, 80.f});
            m_background->setColor(cocos2d::ccColor3B{50, 50, 50});
            m_background->setZOrder(-1);
            this->addChildAtPosition(m_background, geode::Anchor::Right, { -70.f, 0.f });

            auto spr = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
            spr->setScale(0.6f);
            auto arrowBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ComboComponentNode::right));
            this->addChildAtPosition(arrowBtn, geode::Anchor::Right, { -25.f, 0.f });

            auto arrowBtn2 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ComboComponentNode::left));
            arrowBtn2->setRotation(180.f);
            this->addChildAtPosition(arrowBtn2, geode::Anchor::Right, { -115.f, 0.f });

            m_valueLabel = cocos2d::CCLabelBMFont::create(m_component->getItems().at(m_component->getValue()).c_str(), "bigFont.fnt");
            m_background->addChildAtPosition(m_valueLabel, geode::Anchor::Center, { 0.f, 0.f });
            updateLabel();

            return true;
        }
    };
}