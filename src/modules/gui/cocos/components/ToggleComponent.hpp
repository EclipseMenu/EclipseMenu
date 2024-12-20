#pragma once
#include "BaseComponentNode.hpp"
#include <modules/gui/cocos/popup/options-popup.hpp>

namespace eclipse::gui::cocos {
    class ToggleComponentNode : public BaseComponentNode<ToggleComponentNode, cocos2d::CCMenu, ToggleComponent, float> {
    protected:
        CCMenuItemToggler* m_toggler = nullptr;
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_extraButton = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;

    public:
        bool init(float width) override {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("toggle-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            m_toggler = geode::cocos::CCMenuItemExt::createTogglerWithStandardSprites(0.7f, [this](auto) {
                auto value = !this->m_component->getValue();
                m_component->setValue(value);
                m_component->triggerCallback(value);
            });
            m_toggler->setAnchorPoint({ 0.5, 0.5f });
            m_toggler->toggle(m_component->getValue());
            this->addChildAtPosition(m_toggler, geode::Anchor::Left, { 15.f, 0.f });

            auto labelSize = width - 35.f;
            auto offset = 0.f;
            if (auto options = m_component->getOptions().lock()) {
                m_extraButton = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName("GJ_optionsBtn02_001.png", 0.5f, [options](auto) {
                    OptionsPopup::create(options)->show();
                });
                m_extraButton->setAnchorPoint({ 0.5, 0.5f });
                this->addChildAtPosition(m_extraButton, geode::Anchor::Right, { -15.f, 0.f });
                labelSize -= 25.f;
                offset = -25.f;
            }

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
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 30.f, 0.f });

            return true;
        }
    };
}