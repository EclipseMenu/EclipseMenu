#pragma once
#include "BaseComponentNode.hpp"

#include <modules/config/config.hpp>
#include <modules/gui/cocos/nodes/CCMenuItemExt.hpp>
#include <modules/gui/components/keybind.hpp>

namespace eclipse::gui::cocos {
    class KeybindComponentNode : public BaseComponentNode<KeybindComponentNode, cocos2d::CCMenu, KeybindComponent, float> {
    protected:
        TranslatedLabel* m_keyName{};
        CCMenuItemSpriteExtra* m_resetBtn{};

        friend class SelectKeybindPopup;
    public:
        bool init(float width);

        void onDelete(CCObject*);
    };

    class SelectKeybindPopup : public geode::Popup {
    public:
        static SelectKeybindPopup* create(keybinds::KeybindProps initialKey, KeybindComponentNode* btn) {
            auto ret = new SelectKeybindPopup();
            if (ret->init(initialKey, btn)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

    private:
        bool init(keybinds::KeybindProps initialKey, KeybindComponentNode* btn) {
            if (!Popup::init(260.f, 120.f, "GJ_square02.png")) return false;

            m_keybindNode = btn;
            m_initialKey = initialKey;

            auto label = TranslatedLabel::create("keybinds.press-key");
            label->setID("title"_spr);
            label->limitLabelWidth(m_size.width - 30.f, 1.f, 0.25f);
            m_mainLayer->addChildAtPosition(label, geode::Anchor::Center);

            this->scheduleUpdate();

            return true;
        }

        void triggerChange(keybinds::KeybindProps key) const {
            auto& component = m_keybindNode->m_component;
            config::set(component->getId(), key);
            component->triggerCallback(key);

            // some callbacks might change the key, so just fetch it again
            key = config::get<keybinds::KeybindProps>(component->getId(), keybinds::Keys::None);
            m_keybindNode->m_keyName->setString(keybinds::keyToString(key));

            if (m_keybindNode->m_resetBtn) {
                m_keybindNode->m_resetBtn->setVisible(key != component->getDefaultKey());
            }
        }

        void onExit() override {
            Popup::onExit();
            if (m_waitingForInput) { // If we were waiting for input, we should cancel the keybind
                this->triggerChange(m_initialKey);
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

            static std::string activeKeybindId;
            static keybinds::KeybindProps releasedKey;
            static bool listenerRegistered = false;

            if (!listenerRegistered) {
                keybinds::Manager::get()->registerGlobalListener([](keybinds::KeyEvent event) {
                    if (activeKeybindId.empty()) return false;
                    if (!event.down) {
                        releasedKey = event.props;
                    }
                    return true;
                });
                listenerRegistered = true;
            }

            activeKeybindId = m_keybindNode->m_component->getId();

            if (keybinds::isKeyDown(keybinds::Keys::Escape)) {
                this->onClose(nullptr);
                releasedKey = {};
                activeKeybindId.clear();
            } else if (releasedKey.key != keybinds::Keys::None) {
                m_waitingForInput = false;
                this->triggerChange(releasedKey);
                this->onClose(nullptr);
                releasedKey = {};
                activeKeybindId.clear();
            }
        }

        geode::Ref<KeybindComponentNode> m_keybindNode;
        keybinds::KeybindProps m_initialKey = keybinds::Keys::None;
        bool m_waitingForInput = false;
        bool m_ignoreInput = false;
    };

    inline bool KeybindComponentNode::init(float width) {
        if (!CCMenu::init()) return false;

        this->setID(fmt::format("keybind-{}"_spr, m_component->getId()));
        this->setContentSize({ width, 28.f });

        auto label = TranslatedLabel::create(m_component->getTitle());
        label->setAnchorPoint({ 0, 0.5f });
        label->limitLabelWidth(width * 0.5f, 1.f, 0.1f);
        label->setID("label"_spr);
        this->addChildAtPosition(label, geode::Anchor::Left, { 5.f, 0.f });

        auto offset = 0.f;
        if (m_component->canDelete()) {
            auto spr = cocos2d::CCSprite::createWithSpriteFrameName("trashbin.png"_spr);
            spr->setScale(0.4f);
            auto deleteBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(KeybindComponentNode::onDelete));
            deleteBtn->setID("delete"_spr);
            offset = -deleteBtn->getContentWidth() / 2;
            this->addChildAtPosition(deleteBtn, geode::Anchor::Right, { offset - 5.f, 0.f });
            offset = offset * 2 - 5.f;
        }

        auto key = config::get<keybinds::KeybindProps>(m_component->getId(), keybinds::Keys::None);
        m_keyName = TranslatedLabel::createRaw(keybinds::keyToString(key));
        m_keyName->setScale(1.2f);

        auto btnWidth = width * 0.3f;
        auto btnSprite = geode::NineSlice::create("square02b_001.png", { 0.0f, 0.0f, 80.0f, 80.0f });
        btnSprite->setContentSize({ btnWidth * (1.f / 0.75f), 36.f });
        btnSprite->setScale(0.6f);
        btnSprite->addChildAtPosition(m_keyName, geode::Anchor::Center);
        m_keyName->limitLabelWidth(btnWidth * (1.f / 0.75f) * 0.9f, 1.2f, 0.1f);

        auto tm = ThemeManager::get();
        btnSprite->setColor(tm->getButtonBackgroundColor().toCCColor3B());
        m_keyName->setColor(tm->getButtonForegroundColor().toCCColor3B());

        offset -= btnWidth / 2.5;

        auto btn = createSpriteExtra(
            btnSprite,
            [this](auto) {
                SelectKeybindPopup::create(
                    config::get<keybinds::KeybindProps>(m_component->getId(), keybinds::Keys::None),
                    this
                )->show();
            }
        );
        btn->m_scaleMultiplier = 1.1f;
        btn->setID("keybind"_spr);
        this->addChildAtPosition(btn, geode::Anchor::Right, { offset - 5.f, 0.f });
        offset -= btnWidth / 2;

        if (m_component->getDefaultKey() != keybinds::Keys::None) {
            m_resetBtn = createSpriteExtraWithFrame(
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

    inline void KeybindComponentNode::onDelete(CCObject*) {
        config::set<keybinds::KeybindProps>(m_component->getId(), keybinds::Keys::None);
        m_keyName->setString(keybinds::keyToString(keybinds::Keys::None));
        m_component->triggerCallback(keybinds::Keys::None);
    }
}
