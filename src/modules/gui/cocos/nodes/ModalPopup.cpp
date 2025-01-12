#include "ModalPopup.hpp"

#include <modules/gui/cocos/cocos.hpp>

#include "FallbackBMFont.hpp"

namespace eclipse::gui::cocos {

    ModalPopup* ModalPopup::create(eclipse::Popup const& settings) {
        auto ret = new ModalPopup();
        if (ret->initAnchored(260.f, 160.f, settings, "GJ_square04.png")) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    bool ModalPopup::setup(eclipse::Popup const &settings) {
        m_closeBtn->setVisible(false);

        m_settings = settings;
        m_bgSprite->setColor({ 100, 100, 100});

        auto title = TranslatedLabel::createRaw(settings.getTitle());
        title->setID("title"_spr);
        title->limitLabelWidth(m_size.width - 30.f, 1.f, 0.25f);
        title->setColor({ 64, 255, 160 });
        m_mainLayer->addChildAtPosition(title, geode::Anchor::Top, { 0.f, -20.f });

        auto message = TranslatedLabel::createWrappedRaw(settings.getMessage(), m_size.width - 30.f, 0.8f);
        message->setID("message"_spr);
        message->setAlignment(BMFontAlignment::Center);
        m_mainLayer->addChildAtPosition(message, geode::Anchor::Center, { 0.f, 10.f });

        auto buttonsMenu = cocos2d::CCMenu::create();
        buttonsMenu->setID("buttons-menu"_spr);
        buttonsMenu->setContentSize({ 200.f, 28.f });
        buttonsMenu->setAnchorPoint({ 0.5f, 0.5f });
        auto btn1 = geode::cocos::CCMenuItemExt::createSpriteExtra(
            createButtonSprite(settings.getButton1()),
            [this](auto) {
                m_settings.getCallback()(true);
                this->onClose(nullptr);
            }
        );
        btn1->setID("button1"_spr);
        buttonsMenu->addChild(btn1);

        if (!settings.getButton2().empty()) {
            m_cancelable = false;
            auto btn2 = geode::cocos::CCMenuItemExt::createSpriteExtra(
                createButtonSprite(settings.getButton2()),
                [this](auto) {
                    m_settings.getCallback()(false);
                    this->onClose(nullptr);
                }
            );
            btn2->setID("button2"_spr);
            buttonsMenu->addChild(btn2);
        }

        buttonsMenu->alignItemsHorizontallyWithPadding(10.f);
        m_mainLayer->addChildAtPosition(buttonsMenu, geode::Anchor::Bottom, { 0.f, 30.f });

        return true;
    }

    void ModalPopup::onExit() {
        Popup::onExit();
        if (auto cocos = CocosRenderer::get())
            cocos->unregisterModal(this);
    }

    cocos2d::CCNode* ModalPopup::createButtonSprite(std::string const& text) const {
        auto label = TranslatedLabel::createRaw(text);
        label->setScale(0.9f);
        auto bg = cocos2d::extension::CCScale9Sprite::create("geode.loader/GE_button_05.png");
        bg->setContentSize({ label->getContentSize().width + 20.f, 28.f });
        bg->setID("bg"_spr);
        bg->setAnchorPoint({ 0.5f, 0.5f });
        bg->addChildAtPosition(label, geode::Anchor::Center);
        return bg;
    }

    void ModalPopup::keyBackClicked() {
        if (m_cancelable) {
            Popup::keyBackClicked();
            m_settings.getCallback()(true);
        }
    }
}
