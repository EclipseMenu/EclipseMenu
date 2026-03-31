#include "ThinkingIndicator.hpp"

using namespace geode::prelude;

namespace eclipse::ai {
    ThinkingIndicator* ThinkingIndicator::create() {
        auto ret = new ThinkingIndicator();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    CCSize const& ThinkingIndicator::getContentSize() const {
        static constexpr CCSize zero{0.f, 0.f};
        if (!m_enabled) {
            return zero;
        }
        return CCNode::getContentSize();
    }

    void ThinkingIndicator::setEnabled(bool enabled) {
        m_enabled = enabled;
        for (auto child : this->getChildrenExt()) {
            child->setVisible(enabled);
        }
    }

    bool ThinkingIndicator::init() {
        if (!CCNode::init()) {
            return false;
        }

        auto dot1 = CCSprite::createWithSpriteFrameName("darkblade_02_color_001.png");
        auto dot2 = CCSprite::createWithSpriteFrameName("darkblade_02_color_001.png");
        auto dot3 = CCSprite::createWithSpriteFrameName("darkblade_02_color_001.png");

        if (!dot1 || !dot2 || !dot3) {
            return false;
        }

        this->setContentSize({25.f, 10.f});

        auto setupDot = [](CCSprite* dot, float x, float t) {
            dot->setScale(0.3f);
            dot->setOpacity(180);
            dot->setAnchorPoint({0.f, 0.f});
            dot->setPosition({x, 0.f});
            dot->runAction(CCSequence::create(
                CCDelayTime::create(t),
                CCRepeat::create(CCSequence::create(
                    CCEaseExponentialInOut::create(CCMoveTo::create(0.5f, {x, 5.f})),
                    CCEaseExponentialInOut::create(CCMoveTo::create(0.5f, {x, 0.f})),
                    nullptr
                ), kCCRepeatForever),
                nullptr
            ));
        };

        setupDot(dot1, 2.f, 0.f);
        setupDot(dot2, 10.f, 0.15f);
        setupDot(dot3, 18.f, 0.3f);

        this->addChild(dot1);
        this->addChild(dot2);
        this->addChild(dot3);

        auto label = CCLabelBMFont::create("Thinking...", "chatFont.fnt");
        if (!label) return false;

        label->setAnchorPoint({0.f, 0.5f});
        label->setPosition({30.f, 5.f});
        label->setScale(0.5f);

        this->setContentWidth(label->getScaledContentSize().width + 30.f);
        this->addChild(label);
        this->setEnabled(false);

        return true;
    }
}
