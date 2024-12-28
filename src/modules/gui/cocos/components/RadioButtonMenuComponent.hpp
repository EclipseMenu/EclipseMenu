#pragma once
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <modules/gui/popup.hpp>
#include <modules/gui/theming/manager.hpp>

namespace eclipse::gui::cocos {
    class RadioButtonsMenuNode : public cocos2d::CCNode {
    protected:
        std::vector<std::shared_ptr<RadioButtonComponent>> m_radioButtons;
        std::vector<std::pair<CCMenuItemToggler*, int>> m_toggles;

    public:
        cocos2d::CCSprite* createRadioButton(bool check) {
            const auto tm = ThemeManager::get();
            auto box = cocos2d::CCSprite::create("circle.png"_spr);
            box->setScale(1.3F);
            if (check) {
                auto checkmark = cocos2d::CCSprite::create("circle.png"_spr);
                checkmark->setScale(0.7F);
                box->addChildAtPosition(checkmark, geode::Anchor::Center);
                checkmark->setColor(tm->getCheckboxCheckmarkColor().toCCColor3B());
            }
            box->setColor(tm->getCheckboxBackgroundColor().toCCColor3B());
            return box;
        }
        void addRadioButton(std::shared_ptr<RadioButtonComponent> const& radioButton, float width) {
            const auto tm = ThemeManager::get();
            constexpr float height = 28.f;

            auto node = cocos2d::CCMenu::create();
            node->setContentSize({ width, height });

            auto choice = radioButton->getChoice();
            // 0.7
            auto toggle = geode::cocos::CCMenuItemExt::createToggler(
                createRadioButton(true), createRadioButton(false), [this, radioButton, choice](auto) {
                    radioButton->setValue(choice);
                    radioButton->triggerCallback(choice);

                    // Update all toggles
                    for (auto const& [t, c] : m_toggles) {
                        t->setEnabled(c != choice);
                        t->toggle(c == choice);
                    }
                }
            );
            toggle->setAnchorPoint({ 0.5, 0.5f });
            toggle->toggle(radioButton->getValue() == choice);
            node->addChildAtPosition(toggle, geode::Anchor::Left, { 15.f, 0.f });

            m_toggles.emplace_back(toggle, choice);

            auto label = TranslatedLabel::create(radioButton->getTitle());
            label->setAnchorPoint({ 0, 0.5f });
            label->limitLabelWidth(width - 30.f, 1.f, 0.25f);
            label->setColor(tm->getCheckboxForegroundColor().toCCColor3B());
            node->addChildAtPosition(label, geode::Anchor::Left, { 30.f, 0.f });

            node->setID(fmt::format("option-{}"_spr, choice));
            this->addChild(node, 1, choice);
        }

        bool init(float width) {
            if (!CCNode::init()) return false;
            this->setID(fmt::format("radio-group-{}"_spr, m_radioButtons[0]->getId()));

            for (auto const& radioButton : m_radioButtons) {
                addRadioButton(radioButton, width);
            }

            this->setContentSize({ width, 28.f * m_radioButtons.size() });

            this->setLayout(
                geode::ColumnLayout::create()
                    ->setGap(0.f)
                    ->setAutoScale(false)
                    ->setAxisReverse(true)
                    ->setAutoGrowAxis(this->getContentHeight())
                    ->setAxisAlignment(geode::AxisAlignment::End)
            );

            return true;
        }

        static RadioButtonsMenuNode* create(std::vector<std::shared_ptr<RadioButtonComponent>> radioButtons, float width) {
            auto ret = new RadioButtonsMenuNode;
            ret->m_radioButtons = std::move(radioButtons);
            if (ret->init(width)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
    };
}
