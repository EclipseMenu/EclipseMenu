#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/components/input-int.hpp>

namespace eclipse::gui::cocos {
    class InputIntComponentNode : public BaseComponentNode<InputIntComponentNode, cocos2d::CCMenu, InputIntComponent, float>, public TextInputDelegate {
    protected:
        TranslatedLabel* m_label = nullptr;
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

            m_textInput = geode::TextInput::create(120, m_component->getTitle().c_str(), "font_default.fnt"_spr);
            m_textInput->setAnchorPoint({ 0.5f, 0.5f });
            m_textInput->getInputNode()->setAllowedChars("0123456789");
            m_textInput->setDelegate(this);
            m_textInput->setString(std::to_string(m_component->getValue()));
            m_textInput->setScale(0.85f);
            this->addChildAtPosition(m_textInput, geode::Anchor::Right, { -70.f, 0.f });

            return true;
        }

        virtual void textChanged(CCTextInputNode* input) override {
            geode::Result<int> valueOpt = geode::utils::numFromString<int>(input->getString());
            if(!valueOpt)
                return;

            int value = std::clamp(*valueOpt, m_component->getMin(), m_component->getMax());
            
            m_component->setValue(value);
            m_component->triggerCallback(value);
        }
        
        void textInputClosed(CCTextInputNode* input) override {
            m_textInput->setString(std::to_string(m_component->getValue()));
        }
    };
}