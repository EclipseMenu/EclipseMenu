#pragma once
#include <Geode/ui/Button.hpp>

#include "MessageBubble.hpp"
#include "ThinkingIndicator.hpp"
#include "../ai/Chatbot.hpp"

namespace eclipse::ai {
    class ChatboxPopup : public geode::Popup {
    public:
        static ChatboxPopup* create();

        void addMessage(geode::ZStringView text, bool isMine);

    protected:
        bool init() override;
        void submitMessage(geode::ZStringView text);
        void generateResponse(std::string prompt);

        void lockUI();
        void unlockUI();

    private:
        Chatbot m_chatbot;

        std::vector<geode::Ref<MessageBubble>> m_messages;

        geode::TextInput* m_input = nullptr;
        geode::Button* m_sendButton = nullptr;
        geode::Scrollbar* m_scrollbar = nullptr;
        geode::ScrollLayer* m_chatHistory = nullptr;
        ThinkingIndicator* m_thinkingIndicator = nullptr;

        bool m_isGenerating = false;
    };
}
