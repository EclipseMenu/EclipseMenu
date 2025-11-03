#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/components/float-toggle.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::cocos {
    class FloatToggleComponentNode : public BaseComponentNode<FloatToggleComponentNode, cocos2d::CCMenu, FloatToggleComponent, float>, public TextInputDelegate {
    protected:
        CCMenuItemToggler* m_toggler = nullptr;
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;
        geode::TextInput* m_textInput = nullptr;

    public:
        bool init(float width) {
            if (!CCMenu::init()) return false;
            auto const tm = ThemeManager::get();

            this->setID(fmt::format("toggle-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            m_toggler = geode::cocos::CCMenuItemExt::createToggler(
                createButton("checkmark.png"_spr), createButton(nullptr),
                [this](auto) {
                auto value = !this->m_component->getState();
                m_component->setState(value);
                m_component->triggerCallback();
            });
            m_toggler->setAnchorPoint({ 0.5, 0.5f });
            m_toggler->toggle(m_component->getState());
            this->addChildAtPosition(m_toggler, geode::Anchor::Left, { 15.f, 0.f });

            auto labelSize = (width * 0.6f) - 35.f;

            if (!m_component->getDescription().empty()) {
                auto spr = cocos2d::CCSprite::createWithSpriteFrameName("info.png"_spr);
                spr->setScale(0.35f);
                m_infoButton = CCMenuItemSpriteExtra::create(
                    spr, this,
                    menu_selector(BaseComponentNode::openDescriptionPopup)
                );
                m_infoButton->setAnchorPoint({ 0.5, 0.5f });
                m_infoButton->setColor(tm->getCheckboxCheckmarkColor().toCCColor3B());
                this->addChildAtPosition(m_infoButton, geode::Anchor::Right, { -10.f, 0.f });
                labelSize -= 15.f;
            }

            m_label = TranslatedLabel::create(m_component->getTitle());
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            m_label->setColor(tm->getCheckboxForegroundColor().toCCColor3B());
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 30.f, 0.f });

            m_textInput = geode::TextInput::create(120, m_component->getTitle().c_str(), "font_default.fnt"_spr);
            m_textInput->setAnchorPoint({ 0.5f, 0.5f });
            m_textInput->getInputNode()->setAllowedChars(".0123456789");
            m_textInput->setDelegate(this);
            m_textInput->setString(fmt::to_string(m_component->getValue()));
            m_textInput->setScale(0.85f);
            this->addChildAtPosition(m_textInput, geode::Anchor::Right, { -70.f, 0.f });

            return true;
        }

        void textChanged(CCTextInputNode* input) override {
            geode::Result<float> valueOpt = geode::utils::numFromString<float>(input->getString());
            if(!valueOpt)
                return;

            float value = std::clamp(*valueOpt, m_component->getMin(), m_component->getMax());

            m_component->setValue(value);
            m_component->triggerCallback(value);
        }

        void textInputClosed(CCTextInputNode* input) override {
            m_textInput->setString(fmt::to_string(m_component->getValue()));
        }
    };
}
