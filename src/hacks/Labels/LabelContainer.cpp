/*#include "LabelContainer.hpp"

#include <algorithm>
#include <modules/config/config.hpp>

namespace eclipse::hacks::Labels {

    void LabelsContainer::addLabel(Label *label, const std::function<void(Label*)> &update) {
        m_labels.emplace_back(label, update);
    }

    void LabelsContainer::removeLabel(Label *label) {
        m_labels.erase(std::remove_if(m_labels.begin(), m_labels.end(), [label](const auto &pair) {
            return pair.first == label;
        }), m_labels.end());
    }

    void LabelsContainer::update() {
        if (!PlayLayer::get()) return;

        recalculatePositions();

        if (!m_visible) {
            for (auto &label: m_labels) {
                label.first->setVisible(false);
            }
            return;
        }

        for (auto &label: m_labels) {
            label.second(label.first);
        }
    }

    void LabelsContainer::recalculatePositions() {
        PlayLayer* playLayer = PlayLayer::get();
        if (!playLayer) return;

        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        auto padding = config::get<float>("hack.labels.padding");
        cocos2d::CCPoint anchor = {0, 0};
        cocos2d::CCPoint offset = {0, 0};
        float verticalSign = 1.0f;

        switch (m_alignment) {
            case Alignment::TopLeft:
                anchor = ccp(0, 1);
                offset = ccp(padding, winSize.height - padding);
                break;
            case Alignment::TopCenter:
                anchor = ccp(0.5, 1);
                offset = ccp(winSize.width / 2, winSize.height - padding);
                break;
            case Alignment::TopRight:
                anchor = ccp(1, 1);
                offset = ccp(winSize.width - padding, winSize.height - padding);
                break;
            case Alignment::BottomLeft:
                anchor = ccp(0, 0);
                offset = ccp(padding, padding);
                verticalSign = -1.0f;
                break;
            case Alignment::BottomCenter:
                anchor = ccp(0.5, 0);
                offset = ccp(winSize.width / 2, padding);
                verticalSign = -1.0f;
                break;
            case Alignment::BottomRight:
                anchor = ccp(1, 0);
                offset = ccp(winSize.width - padding, padding);
                verticalSign = -1.0f;
                break;
        }

        for (auto &label: m_labels) {
            if (!label.first->isVisible()) continue;

            label.first->setAnchor(anchor);
            label.first->setPosition(offset);

            offset.y -= label.first->getHeight() * verticalSign * label.first->getScale();
        }
    }

    void LabelsContainer::clear() {
        for (auto &label: m_labels) {
            delete label.first;
        }
        m_labels.clear();
    }


}*/