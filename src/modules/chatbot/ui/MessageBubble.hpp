#pragma once
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <rock/RoundedRect.hpp>

namespace eclipse::ai {
    class MessageBubble : public rock::RoundedRect {
    public:
        static MessageBubble* create(geode::ZStringView text, bool isMine);
        static MessageBubble* createAnimated(std::string text, bool isMine, geode::Function<void()> onComplete = nullptr);

        void setText(geode::ZStringView text);

    protected:
        bool init(geode::ZStringView text, bool isMine);
        void update(float dt) override;

    private:
        gui::cocos::EmojiLabel* m_label = nullptr;
        geode::Function<void()> m_onComplete;
        std::string m_text;
        size_t m_currentChar = 0;
        float m_nextCharTime = 0.f;
    };
}