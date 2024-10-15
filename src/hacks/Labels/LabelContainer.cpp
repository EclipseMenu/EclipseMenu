#include "LabelContainer.hpp"

#include <algorithm>
#include <modules/config/config.hpp>

namespace eclipse::hacks::Labels {

    bool LabelsContainer::init(Alignment alignment) {
        if (!cocos2d::CCNode::init()) return false;

        m_alignment = alignment;
        updatePosition();

        auto* layout = cocos2d::AxisLayout::create(cocos2d::Axis::Column);
        layout->setAxisReverse(true);
        layout->setAutoScale(false);
        layout->setGrowCrossAxis(false);
        layout->setCrossAxisOverflow(true);
        layout->setGap(0.f);

#define SET_ALIGNMENT(axis, crossAxis, crossAxisLine) \
        layout->setAxisAlignment(cocos2d::AxisAlignment:: axis);               \
        layout->setCrossAxisAlignment(cocos2d::AxisAlignment:: crossAxis);     \
        layout->setCrossAxisLineAlignment(cocos2d::AxisAlignment:: crossAxisLine);

        switch (m_alignment) {
            case Alignment::TopLeft:
                SET_ALIGNMENT(End, Start, Start);
                break;
            case Alignment::TopCenter:
                SET_ALIGNMENT(End, Center, Center);
                break;
            case Alignment::TopRight:
                SET_ALIGNMENT(End, End, End);
                break;
            case Alignment::CenterLeft:
                SET_ALIGNMENT(Center, Start, Start);
                break;
            case Alignment::Center:
                SET_ALIGNMENT(Center, Center, Center);
                break;
            case Alignment::CenterRight:
                SET_ALIGNMENT(Center, End, End);
                break;
            case Alignment::BottomLeft:
                layout->setAxisReverse(false);
                SET_ALIGNMENT(Start, Start, Start);
                break;
            case Alignment::BottomCenter:
                layout->setAxisReverse(false);
                SET_ALIGNMENT(Start, Center, Center);
                break;
            case Alignment::BottomRight:
                layout->setAxisReverse(false);
                SET_ALIGNMENT(Start, End, End);
                break;
        }

        this->setLayout(layout, false);

        return true;
    }

    void LabelsContainer::updatePosition() {
        auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
        auto padding = config::get<float>("labels.padding", 3.f);

        setContentSize({ winSize.width - padding * 2, winSize.height - padding * 2 });

        switch (m_alignment) {
            case Alignment::TopLeft:
                setPosition(padding, winSize.height - padding);
                setAnchorPoint({0, 1});
                break;
            case Alignment::TopCenter:
                setPosition(winSize.width / 2, winSize.height - padding);
                setAnchorPoint({0.5, 1});
                break;
            case Alignment::TopRight:
                setPosition(winSize.width - padding, winSize.height - padding);
                setAnchorPoint({1, 1});
                break;
            case Alignment::CenterLeft:
                setPosition(padding, winSize.height / 2);
                setAnchorPoint({0, 0.5});
                break;
            case Alignment::Center:
                setPosition(winSize.width / 2, winSize.height / 2);
                setAnchorPoint({0.5, 0.5});
                break;
            case Alignment::CenterRight:
                setPosition(winSize.width - padding, winSize.height / 2);
                setAnchorPoint({1, 0.5});
                break;
            case Alignment::BottomLeft:
                setPosition(padding, padding);
                setAnchorPoint({0, 0});
                break;
            case Alignment::BottomCenter:
                setPosition(winSize.width / 2, padding);
                setAnchorPoint({0.5, 0});
                break;
            case Alignment::BottomRight:
                setPosition(winSize.width - padding, padding);
                setAnchorPoint({1, 0});
                break;
        }

        this->updateLayout(false);
    }

    void LabelsContainer::addLabel(SmartLabel* label, const std::function<void(SmartLabel*)>& update) {
        label->setParentContainer(this);
        m_labels.emplace_back(label, update);

        // set anchor point
        switch (m_alignment) {
            case Alignment::TopLeft:
                label->setAnchorPoint({0, 1});
                break;
            case Alignment::TopCenter:
                label->setAnchorPoint({0.5, 1});
                break;
            case Alignment::TopRight:
                label->setAnchorPoint({1, 1});
                break;
            case Alignment::CenterLeft:
                label->setAnchorPoint({0, 0.5});
                break;
            case Alignment::Center:
                label->setAnchorPoint({0.5, 0.5});
                break;
            case Alignment::CenterRight:
                label->setAnchorPoint({1, 0.5});
                break;
            case Alignment::BottomLeft:
                label->setAnchorPoint({0, 0});
                break;
            case Alignment::BottomCenter:
                label->setAnchorPoint({0.5, 0});
                break;
            case Alignment::BottomRight:
                label->setAnchorPoint({1, 0});
                break;
        }

        addChild(label);
    }

    void LabelsContainer::removeLabel(SmartLabel* label) {
        auto it = std::find_if(m_labels.begin(), m_labels.end(), [label](const auto& pair) {
            return pair.first == label;
        });

        if (it != m_labels.end()) {
            m_labels.erase(it);
            removeChild(label);
        }
    }

    void LabelsContainer::update() {
        if (!isVisible()) return;

        for (auto& label : m_labels) {
            label.second(label.first);
            label.first->update();
        }
    }

}