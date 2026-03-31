#include "ChatboxPopup.hpp"

using namespace geode::prelude;

namespace eclipse::ai {
    ChatboxPopup* ChatboxPopup::create() {
        auto ret = new ChatboxPopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    void ChatboxPopup::addMessage(ZStringView text, bool isMine) {
        auto bubble = MessageBubble::create(text, isMine);
        m_messages.push_back(bubble);

        // wrap "mine" messages in a container to align them to the right
        CCNode* node = bubble;
        if (isMine) {
            auto container = CCNode::create();
            container->addChild(bubble);
            container->setContentHeight(bubble->getContentHeight());
            container->setContentWidth(170.f);
            bubble->setAnchorPoint({1.f, 0.f});
            bubble->setPositionX(170.f);
            node = container;
        }

        m_chatHistory->m_contentLayer->addChild(node);
        m_chatHistory->m_contentLayer->updateLayout();
    }

    bool ChatboxPopup::init() {
        if (!Popup::init(200.f, 260.f, "geode.loader/GE_square03.png")) {
            return false;
        }

        this->setID("chatbox-popup"_spr);

        m_input = TextInput::create(150.f, "Type a message...", "chatFont.fnt");
        if (!m_input) return false;

        m_chatHistory = ScrollLayer::create({170.f, 205.f});
        if (!m_chatHistory) return false;

        m_scrollbar = Scrollbar::create(m_chatHistory);
        if (!m_scrollbar) return false;

        auto sendButtonIcon = CCSprite::createWithSpriteFrameName("d_sign_img_02_001.png");
        if (!sendButtonIcon) return false;

        sendButtonIcon->setRotation(-90.f);
        auto sendButtonBase = EditorButtonSprite::create(sendButtonIcon, EditorBaseColor::Green, EditorBaseSize::Normal);
        if (!sendButtonBase) return false;

        sendButtonBase->setScale(0.8f);
        sendButtonIcon->setScale(1.f);

        m_sendButton = Button::createWithNode(sendButtonBase, [this](Button*) {
            this->submitMessage(m_input->getString());
        });
        if (!m_sendButton) return false;

        m_input->setCommonFilter(CommonFilter::Any);
        this->addEventListener(KeyboardInputEvent(KEY_Enter), [this](KeyboardInputData& event) {
            if (event.action == KeyboardInputData::Action::Release && m_input->getInputNode()->m_selected) {
                this->submitMessage(m_input->getString());
                m_input->defocus();
            }
        });

        m_input->setAnchorPoint({0.f, 0.f});
        m_scrollbar->setAnchorPoint({1.f, 1.f});

        m_thinkingIndicator = ThinkingIndicator::create();
        if (!m_thinkingIndicator) return false;
        m_thinkingIndicator->setZOrder(10);
        m_chatHistory->m_contentLayer->addChild(m_thinkingIndicator);

        m_chatHistory->m_contentLayer->setLayout(
            SimpleAxisLayout::create(Axis::Column)
                ->setGap(5.f)
                ->ignoreInvisibleChildren(false)
                ->setMainAxisAlignment(MainAxisAlignment::End)
                ->setMainAxisScaling(AxisScaling::Grow)
                ->setCrossAxisAlignment(CrossAxisAlignment::Start)
        );

        m_input->setID("input-box");
        m_chatHistory->setID("chat-history");
        m_scrollbar->setID("scrollbar");
        m_sendButton->setID("send-button");
        m_thinkingIndicator->setID("thinking-indicator");

        m_mainLayer->addChildAtPosition(m_input, Anchor::BottomLeft, { 10.f, 10.f });
        m_mainLayer->addChildAtPosition(m_chatHistory, Anchor::BottomLeft, { 10.f, 45.f });
        m_mainLayer->addChildAtPosition(m_scrollbar, Anchor::TopRight, { -8.f, -10.f });
        m_mainLayer->addChildAtPosition(m_sendButton, Anchor::BottomRight, { -22.5f, 25.f });

        auto res = m_chatbot.loadConfig(Mod::get()->getSaveDir() / "chatbot.json");
        if (!res) {
            log::error("Failed to load chatbot config: {}", res.unwrapErr());
            return false;
        }

        return true;
    }

    void ChatboxPopup::submitMessage(ZStringView text) {
        if (m_isGenerating || text.empty()) return;

        this->addMessage(text, true);

        this->lockUI();
        m_thinkingIndicator->setEnabled(true);
        m_chatHistory->m_contentLayer->updateLayout();
        m_chatHistory->m_contentLayer->setPositionY(0.f); // scroll to bottom

        this->runAction(CCSequence::create(
            CCDelayTime::create(random::generate(1.f, 2.f)),
            CallFuncExt::create([this, msg = std::string(text)] mutable {
                m_thinkingIndicator->setEnabled(false);
                this->generateResponse(std::move(msg));
            }),
            nullptr
        ));

        m_input->setString("");
    }

    void ChatboxPopup::generateResponse(std::string prompt) {
        auto reply = m_chatbot.process(prompt);

        auto bubble = MessageBubble::createAnimated(std::move(reply), false, [this] {
            this->unlockUI();
        });

        m_messages.push_back(bubble);
        m_chatHistory->m_contentLayer->addChild(bubble);
        m_chatHistory->m_contentLayer->updateLayout();
    }

    void ChatboxPopup::lockUI() {
        m_isGenerating = true;
        m_input->setEnabled(false);
        m_sendButton->setEnabled(false);
    }

    void ChatboxPopup::unlockUI() {
        m_isGenerating = false;
        m_input->setEnabled(true);
        m_sendButton->setEnabled(true);
    }
}