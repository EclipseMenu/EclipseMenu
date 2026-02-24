#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/cocos/nodes/CCMenuItemExt.hpp>
#include <modules/gui/cocos/popup/label-settings-popup.hpp>
#include <modules/gui/components/label-settings.hpp>
#include <modules/labels/setting.hpp>

namespace eclipse::gui::cocos {
    class LabelSettingsComponentNode : public BaseComponentNode<LabelSettingsComponentNode, cocos2d::CCMenu, LabelSettingsComponent, float> {
    protected:
        CCMenuItemToggler* m_toggler = nullptr;

    public:
        bool init(float width) {
            if (!CCMenu::init()) return false;

            auto const tm = ThemeManager::get();
            constexpr auto height = 36.f;
            auto settings = m_component->getSettings();

            this->setID(fmt::format("setting-{}"_spr, m_component->getId()));
            this->setContentSize({width, height});

            auto bg = geode::NineSlice::create("square02b_001.png");
            bg->setScale(0.5f);
            bg->setContentSize({width * 2 - 10.f, height * 2});
            bg->setColor(tm->getFrameBackground().toCCColor3B());
            bg->setOpacity(90);
            bg->setID("background");
            this->addChildAtPosition(bg, geode::Anchor::Center, {0, 0});

            m_toggler = createToggler(
                createButton("checkmark.png"_spr),
                createButton(nullptr),
                [this, settings](auto) {
                    settings->visible = !settings->visible;
                    this->m_component->triggerEditCallback();
                }
            );
            m_toggler->setAnchorPoint({ 0.5, 0.5f });
            m_toggler->toggle(m_component->getSettings()->visible);
            this->addChildAtPosition(m_toggler, geode::Anchor::Left, { 20.f, 0.f });

            auto nameTextBox = geode::TextInput::create(120.f, "Label Name", "font_default.fnt"_spr);
            nameTextBox->setString(settings->name);
            nameTextBox->setID("name-textbox");
            nameTextBox->setAnchorPoint({0, 0.5f});
            nameTextBox->setCallback([this](std::string const& text) {
                m_component->getSettings()->name = text;
                this->m_component->triggerEditCallback();
            });
            this->addChildAtPosition(nameTextBox, geode::Anchor::Left, { 35.f, 0.f });

            auto deleteButton = createSpriteExtra(
                createButton("trashbin.png"_spr, 0.5f),
                [this](auto) {
                    this->m_component->triggerDeleteCallback();
                }
            );
            deleteButton->setID("delete-btn");
            this->addChildAtPosition(deleteButton, geode::Anchor::Right, { -21.f, 0.f });

            auto settingsButton = createSpriteExtra(
                createButton("settings.png"_spr, 0.5f),
                [this](auto) {
                    LabelSettingsPopup::create(m_component)->show();
                }
            );
            settingsButton->setID("settings-btn");
            this->addChildAtPosition(settingsButton, geode::Anchor::Right, { -54.f, 0.f });

            auto const createArrowBtn = [&](bool flip) {
                auto arrow = cocos2d::CCSprite::createWithSpriteFrameName("arrow.png"_spr);
                if (flip) arrow->setFlipY(true);
                arrow->setScale(0.5f);
                arrow->setColor(tm->getCheckboxCheckmarkColor().toCCColor3B());
                return arrow;
            };

            auto moveBg = geode::NineSlice::create("square02b_001.png");
            moveBg->setScale(0.475f);
            moveBg->setContentSize({120.f, height * 2 - 10.f});
            moveBg->setColor(tm->getFrameBackground().toCCColor3B());
            moveBg->setOpacity(90);
            moveBg->setID("move-arrows-background");
            this->addChildAtPosition(moveBg, geode::Anchor::Center, {12.5, 0});

            auto moveUpBtn = createSpriteExtra(
                createArrowBtn(false),
                [this](auto) {
                    m_component->triggerMoveCallback(true);
                }
            );
            moveUpBtn->setID("move-up-btn");
            this->addChildAtPosition(moveUpBtn, geode::Anchor::Center, { 25.f, 0.f });

            auto moveDownBtn = createSpriteExtra(
                createArrowBtn(true),
                [this](auto) {
                    m_component->triggerMoveCallback(false);
                }
            );
            moveDownBtn->setID("move-down-btn");
            this->addChildAtPosition(moveDownBtn, geode::Anchor::Center, { 0.f, 0.f });

            return true;
        }
    };
}
