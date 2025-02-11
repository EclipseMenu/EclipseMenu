#pragma once
#include "BaseComponentNode.hpp"

#include <modules/config/config.hpp>
#include <modules/gui/components/keybind.hpp>

namespace eclipse::gui::cocos {
    class SelectKeybindPopup : public geode::Popup<keybinds::Keys, std::function<void(keybinds::Keys)> const&> {
    public:
        static SelectKeybindPopup* create(keybinds::Keys initialKey, std::function<void(keybinds::Keys)> const& callback) {
            auto ret = new SelectKeybindPopup();
            if (ret->initAnchored(260.f, 120.f, initialKey, callback, "GJ_square02.png")) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    private:
        bool setup(keybinds::Keys initialKey, std::function<void(keybinds::Keys)> const& callback) override {
            m_callback = callback;
            m_initialKey = initialKey;

            auto label = TranslatedLabel::create("keybinds.press-key");
            label->setID("title"_spr);
            label->limitLabelWidth(m_size.width - 30.f, 1.f, 0.25f);
            m_mainLayer->addChildAtPosition(label, geode::Anchor::Center);

            this->scheduleUpdate();

            return true;
        }

        void onExit() override {
            Popup::onExit();
            if (m_waitingForInput) { // If we were waiting for input, we should cancel the keybind
                m_callback(m_initialKey);
            }
        }

        void update(float) override {
            // do not allow input if the close button is selected
            if (m_closeBtn->m_bSelected) {
                m_ignoreInput = true;
                return;
            }

            if (m_ignoreInput) {
                // wait until the mouse button is released
                m_ignoreInput = keybinds::isKeyDown(keybinds::Keys::MouseLeft);
                return;
            }

            auto from = keybinds::Keys::A;
            auto to = keybinds::Keys::LastKey;
            for (auto i = from; i < to; ++i) {
                // touchscreen users probably don't want to bind left mouse button (a.k.a. any screen tap)
                GEODE_MOBILE(if (i == keybinds::Keys::MouseLeft) continue;)

                if (keybinds::isKeyDown(i)) {
                    m_waitingForInput = false;
                    m_callback(i);
                    this->onClose(nullptr);
                    break;
                }
            }
        }

        keybinds::Keys m_initialKey = keybinds::Keys::None;
        bool m_waitingForInput = false;
        bool m_ignoreInput = false;
        std::function<void(keybinds::Keys)> m_callback;
    };

    class KeybindComponentNode : public BaseComponentNode<KeybindComponentNode, cocos2d::CCMenu, KeybindComponent, float> {
    protected:
        TranslatedLabel* m_keyName{};
        CCMenuItemSpriteExtra* m_resetBtn{};

    public:
        bool init(float width) {
            if (!CCMenu::init()) return false;

            this->setID(fmt::format("keybind-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            auto label = TranslatedLabel::create(m_component->getTitle());
            label->setAnchorPoint({ 0, 0.5f });
            label->limitLabelWidth(width * 0.5f, 1.f, 0.1f);
            this->addChildAtPosition(label, geode::Anchor::Left, { 5.f, 0.f });

            auto offset = 0.f;
            if (m_component->canDelete()) {
                auto deleteBtn = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName(
                    "trashbin.png"_spr, 0.4f,
                    [this](auto) {
                        config::set(m_component->getId(), keybinds::Keys::None);
                        m_keyName->setString(keybinds::keyToString(keybinds::Keys::None));
                        m_component->triggerCallback(keybinds::Keys::None);
                    }
                );
                deleteBtn->setID("delete"_spr);
                offset = -deleteBtn->getContentWidth() / 2;
                this->addChildAtPosition(deleteBtn, geode::Anchor::Right, { offset - 5.f, 0.f });
                offset = offset * 2 - 5.f;
            }

            auto key = config::get<keybinds::Keys>(m_component->getId(), keybinds::Keys::None);
            m_keyName = TranslatedLabel::createRaw(keybinds::keyToString(key));
            m_keyName->setScale(1.2f);

            auto btnWidth = width * 0.3f;
            auto btnSprite = cocos2d::extension::CCScale9Sprite::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
            btnSprite->setContentSize({ btnWidth * (1.f / 0.75f), 36.f });
            btnSprite->setScale(0.6f);
            btnSprite->addChildAtPosition(m_keyName, geode::Anchor::Center);

            auto tm = ThemeManager::get();
            btnSprite->setColor(tm->getButtonBackgroundColor().toCCColor3B());
            m_keyName->setColor(tm->getButtonForegroundColor().toCCColor3B());

            offset -= btnWidth / 2.5;

            auto btn = geode::cocos::CCMenuItemExt::createSpriteExtra(
                btnSprite,
                [this](auto) {
                    SelectKeybindPopup::create(
                        config::get<keybinds::Keys>(m_component->getId(), keybinds::Keys::None),
                        [ref = geode::Ref(this)](auto key) {
                            auto& component = ref->m_component;
                            config::set(component->getId(), key);
                            component->triggerCallback(key);

                            // some callbacks might change the key, so just fetch it again
                            key = config::get<keybinds::Keys>(component->getId(), keybinds::Keys::None);
                            ref->m_keyName->setString(keybinds::keyToString(key));

                            if (ref->m_resetBtn) {
                                ref->m_resetBtn->setVisible(key != component->getDefaultKey());
                            }
                        }
                    )->show();
                }
            );
            btn->m_scaleMultiplier = 1.1f;
            this->addChildAtPosition(btn, geode::Anchor::Right, { offset - 5.f, 0.f });
            offset -= btnWidth / 2;

            if (m_component->getDefaultKey() != keybinds::Keys::None) {
                m_resetBtn = geode::cocos::CCMenuItemExt::createSpriteExtraWithFrameName(
                    "reset.png"_spr, 0.3f,
                    [this](auto) {
                        auto key = m_component->getDefaultKey();
                        config::set(m_component->getId(), key);
                        m_keyName->setString(keybinds::keyToString(key));
                        m_component->triggerCallback(key);
                        m_resetBtn->setVisible(false);
                    }
                );
                m_resetBtn->setID("reset"_spr);
                m_resetBtn->setVisible(key != m_component->getDefaultKey());
                this->addChildAtPosition(m_resetBtn, geode::Anchor::Right, { offset - 15.f, 0.f });
            }

            return true;
        }
    };
}