#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/components/input-text.hpp>

namespace eclipse::gui::cocos {
    class InputTextComponentNode : public BaseComponentNode<InputTextComponentNode, cocos2d::CCMenu, InputTextComponent, float>, public TextInputDelegate {
    protected:
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;
        geode::TextInput* m_textInput = nullptr;

    public:
        bool init(float width) override {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("toggle-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            auto labelSize = (width * 0.5f) - 35.f;

            if (!m_component->getDescription().empty()) {
                m_infoButton = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName("GJ_infoIcon_001.png", 0.5f, [this](auto) {
                    FLAlertLayer::create(
                        m_component->getTitle().c_str(), m_component->getDescription().c_str(), "OK"
                    )->show();
                });
                m_infoButton->setAnchorPoint({ 0.5, 0.5f });
                this->addChildAtPosition(m_infoButton, geode::Anchor::Right, { -10.f, 0.f });
                labelSize -= 15.f;
            }

            m_label = TranslatedLabel::create(m_component->getTitle());
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 15.f, 0.f });

            auto inputWidth = width - labelSize - 50.f;
            m_textInput = geode::TextInput::create(inputWidth, "", "font_default.fnt"_spr);
            m_textInput->setAnchorPoint({ 0.5f, 0.5f });
            m_textInput->setDelegate(this);
            m_textInput->setString(m_component->getValue());
            m_textInput->setScale(0.85f);
            this->addChildAtPosition(m_textInput, geode::Anchor::Right, { -inputWidth / 2 - 5.f, 0.f });

            return true;
        }

        virtual void textChanged(CCTextInputNode* input) override {
            m_component->setValue(input->getString());
            m_component->triggerCallback(input->getString());
        }

        void textInputClosed(CCTextInputNode* input) override {
            m_textInput->setString(m_component->getValue());
        }
    };
}