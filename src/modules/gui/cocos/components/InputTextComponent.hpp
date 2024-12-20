#pragma once
#include "BaseComponentNode.hpp"
#include <modules/gui/cocos/popup/options-popup.hpp>

namespace eclipse::gui::cocos {
    class InputTextComponentNode : public BaseComponentNode<InputTextComponentNode, cocos2d::CCMenu, InputTextComponent, float>, public TextInputDelegate {
    protected:
        cocos2d::CCLabelBMFont* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;
        geode::TextInput* m_textInput = nullptr;

    public:
        bool init(float width) override {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("toggle-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            auto labelSize = (width * 0.6f) - 35.f;

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

            m_label = cocos2d::CCLabelBMFont::create(m_component->getTitle().c_str(), "bigFont.fnt");
            m_label->setAnchorPoint({0, 0.5f});
            m_label->setScale(0.6f);
            m_label->limitLabelWidth(labelSize, 0.6f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 15.f, 0.f });

            m_textInput = geode::TextInput::create(120, m_component->getTitle().c_str());
            m_textInput->setAnchorPoint({ 0.5f, 0.5f });
            m_textInput->setDelegate(this);
            m_textInput->setString(m_component->getValue());
            this->addChildAtPosition(m_textInput, geode::Anchor::Right, { -70.f, 0.f });

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