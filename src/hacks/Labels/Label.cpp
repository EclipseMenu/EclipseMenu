/*
#include "Label.hpp"
#include <utility>

namespace eclipse::hacks::Labels {

    Label::Label(std::string text, std::string font)
        : m_text(std::move(text)), m_font(std::move(font)) {}

    void Label::addToLayer(cocos2d::CCLayer* layer) {
        if (m_layer) removeFromLayer();

        m_label = cocos2d::CCLabelBMFont::create(m_text.c_str(), m_font.c_str());
        m_label->setZOrder(1000);
        m_label->setID(m_id);
        m_layer = layer;
        m_layer->addChild(m_label);
        update();
    }

    void Label::removeFromLayer() {
        if (!m_layer || !m_label) return;

        m_layer->removeChild(m_label);
        CC_SAFE_DELETE(m_label);
    }

    void Label::update() {
        if (!m_label) return;
        m_label->setFntFile(m_font.c_str());
        m_label->setCString(m_text.c_str());
        m_label->setVisible(m_visible);
        m_label->setScale(m_scale);

        if (m_heightMultiplier != 1.0f && m_anchor.y == 0) {
            auto offset = m_label->getContentSize().height - getHeight();
            m_label->setPosition({m_position.x, m_position.y + offset * m_scale});
        } else {
            m_label->setPosition(m_position);
        }

        m_label->setAnchorPoint(m_anchor);
        m_label->setColor(m_color.toCCColor3B());
        m_label->setOpacity(static_cast<uint8_t>(m_color.a * 255));
    }

    float Label::getHeight() const {
        if (!m_label) return 0;
        return m_label->getContentSize().height * m_heightMultiplier;
    }

}*/