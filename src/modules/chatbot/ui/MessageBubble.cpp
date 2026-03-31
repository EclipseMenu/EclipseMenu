#include "MessageBubble.hpp"

using namespace geode::prelude;

namespace eclipse::ai {
    MessageBubble* MessageBubble::create(ZStringView text, bool isMine) {
        auto ret = new MessageBubble();
        if (ret->init(text, isMine)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    MessageBubble* MessageBubble::createAnimated(std::string text, bool isMine, Function<void()> onComplete) {
        auto bubble = create("", isMine);
        if (!bubble) return nullptr;

        bubble->m_onComplete = std::move(onComplete);
        bubble->m_text = std::move(text);
        bubble->scheduleUpdate();

        return bubble;
    }

    void MessageBubble::setText(ZStringView text) {
        m_label->setString(text);
        m_label->limitLabelWidth(160.f, .7f, .1f);
        auto size = m_label->getScaledContentSize() + CCSize{10.f, 10.f};
        this->setContentSize(size);
    }

    bool MessageBubble::init(ZStringView text, bool isMine) {
        auto color = isMine
            ? ccc4(0, 105, 46, 255)
            : ccc4(41, 55, 149, 255);

        m_label = gui::cocos::EmojiLabel::create(text, "chatFont.fnt");
        if (!m_label) return false;

        m_label->setWrapWidth(150.f);
        m_label->setWrapEnabled(true);

        m_label->setAnchorPoint({0.f, 0.5});
        m_label->limitLabelWidth(160.f, .7f, .1f);

        auto size = m_label->getScaledContentSize() + CCSize{10.f, 10.f};
        auto radii = isMine
            ? rock::Radii{10.f, 10.f, 3.f, 10.f}
            : rock::Radii{10.f, 10.f, 10.f, 3.f};

        if (!RoundedRect::init(color, radii, size)) {
            return false;
        }

        this->addChildAtPosition(m_label, Anchor::Left, {5.f, 0.f});

        return true;
    }

    void MessageBubble::update(float dt) {
        if (m_currentChar >= m_text.length()) {
            if (m_onComplete) {
                m_onComplete();
                m_onComplete = nullptr;
            }
            this->unscheduleUpdate();
            return;
        }

        auto prevChar = m_currentChar;

        m_nextCharTime -= dt;
        while (m_nextCharTime <= 0.f && m_currentChar < m_text.length()) {
            m_nextCharTime += random::generate(0.02f, 0.05f);
            m_currentChar++;
        }

        if (prevChar != m_currentChar) {
            this->setText(m_text.substr(0, m_currentChar));
            this->updateLayout();

            if (auto parent = this->getParent()) {
                parent->updateLayout();
            }
        }
    }
}
