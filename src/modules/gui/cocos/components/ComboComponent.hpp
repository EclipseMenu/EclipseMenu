#pragma once
#include "BaseComponentNode.hpp"

#include <modules/gui/components/combo.hpp>
#include <modules/gui/theming/manager.hpp>

#include <modules/gui/cocos/popup/scroll-layer.hpp>

namespace eclipse::gui::cocos {
    class ComboComponentNode : public BaseComponentNode<ComboComponentNode, cocos2d::CCMenu, ComboComponent, float> {
    protected:
        TranslatedLabel* m_label = nullptr;
        CCMenuItemSpriteExtra* m_infoButton = nullptr;
        cocos2d::extension::CCScale9Sprite* m_background = nullptr;
        TranslatedLabel* m_valueLabel = nullptr;
        geode::Ref<CCMenuItemSpriteExtra> m_arrowButton = nullptr;
        geode::Ref<ScrollLayer> m_scrollLayer;

        static inline ComboComponentNode* s_activeCombo = nullptr;
        std::vector<TranslatedLabel*> m_labels;

    public:
        ~ComboComponentNode() override {
            if (s_activeCombo == this) {
                s_activeCombo = nullptr;
            }
        }

        void generateSelector() {
            auto const tm = ThemeManager::get();

            auto const scrollHeight = std::min<float>(m_component->getItems().size(), 3.5f) * 80.f;
            m_scrollLayer = ScrollLayer::create({365.f * 0.3f, scrollHeight * 0.3f});
            m_scrollLayer->setID("scrollLayer");
            m_scrollLayer->setZOrder(2);
            m_scrollLayer->ignoreAnchorPointForPosition(false);
            m_scrollLayer->m_contentLayer->setLayout(
                geode::ColumnLayout::create()
                    ->setAutoScale(false)
                    ->setAxisReverse(true)
                    ->setAutoGrowAxis(m_scrollLayer->getContentHeight())
                    ->setAxisAlignment(geode::AxisAlignment::End)
                    ->setGap(0),
                false
            );

            auto scrollBlock = cocos2d::CCMenu::create();
            scrollBlock->setScale(0.3f);
            scrollBlock->setContentSize({365.f, scrollHeight});
            scrollBlock->setZOrder(-100);
            scrollBlock->registerWithTouchDispatcher();

            auto scrollBlockButton = CCMenuItemSpriteExtra::create(
                cocos2d::CCSprite::create("GJ_button_01.png"),
                nullptr,
                nullptr
            );
            scrollBlockButton->setContentSize({365.f, scrollHeight});
            scrollBlockButton->setZOrder(1);
            scrollBlockButton->setAnchorPoint({0.5f, 0.5f});
            scrollBlockButton->setOpacity(0);
            scrollBlockButton->m_scaleMultiplier = 1.f;
            scrollBlock->addChildAtPosition(scrollBlockButton, geode::Anchor::Center);
            m_scrollLayer->addChildAtPosition(scrollBlock, geode::Anchor::Center);

            auto scrollBackground = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
            scrollBackground->setID("scrollBackground");
            scrollBackground->setScale(0.3f);
            scrollBackground->setContentSize({365.f, scrollHeight});
            scrollBackground->setColor(tm->getFrameBackground().toCCColor3B());
            scrollBackground->setZOrder(-2);
            m_scrollLayer->addChildAtPosition(scrollBackground, geode::Anchor::Center);

            for (size_t i = 0; auto const& component : m_component->getItems()) {
                auto menu = cocos2d::CCMenu::create();
                // menu->registerWithTouchDispatcher();
                menu->setContentSize({365.f * 0.3f, 80.f * 0.3f});

                auto label = TranslatedLabel::createRaw(component);
                label->setAnchorPoint({0.5f, 0.5f});
                label->limitLabelWidth(menu->getContentSize().width - 20.f, 0.75f, 0.25f);
                if (m_component->getValue() == i) {
                    label->setColor(tm->getButtonForegroundColor().toCCColor3B());
                } else {
                    label->setColor(tm->getButtonDisabledForeground().toCCColor3B());
                }
                m_labels.push_back(label);

                auto item = CCMenuItemSpriteExtra::create(label, this, menu_selector(ComboComponentNode::selectItem));
                item->setTag(i);
                item->setAnchorPoint({0.5f, 0.5f});
                item->setZOrder(1);
                item->m_scaleMultiplier = 1.1f;
                menu->addChildAtPosition(item, geode::Anchor::Center);

                m_scrollLayer->m_contentLayer->addChild(menu);

                ++i;
            }

            m_scrollLayer->m_contentLayer->updateLayout();
            m_scrollLayer->scrollToTop();
        }

        void updateLabel() const {
            int index = m_component->getValue();
            if (index < 0 || index >= m_component->getItems().size()) {
                m_valueLabel->setString("");
                return;
            }
            m_valueLabel->setString(m_component->getItems()[index]);
            m_valueLabel->limitLabelWidth(70.f, 1.5f, 0.25f);
        }

        void openSelector(CCObject* sender) {
            auto const backgroundBox = m_background->boundingBox();
            auto const globalTop = m_background->convertToWorldSpace(ccp(backgroundBox.getMidX(), backgroundBox.getMaxY()));
            auto const globalBottom = m_background->convertToWorldSpace(ccp(backgroundBox.getMidX(), backgroundBox.getMinY()));
            auto const winSize = cocos2d::CCDirector::get()->getWinSize();

            m_arrowButton->setRotation(270.f);
            m_arrowButton->setTarget(this, menu_selector(ComboComponentNode::closeSelector));

            if (!m_scrollLayer) {
                this->generateSelector();
            }

            if (s_activeCombo) {
                s_activeCombo->closeSelector(nullptr);
            }
            s_activeCombo = this;

            if (globalBottom.y > m_scrollLayer->getContentHeight() + 20.f) {
                m_scrollLayer->setAnchorPoint({0.5f, 1.f});
                auto const targetPos = ccp(this->getContentWidth() - 70.f, this->getContentHeight() / 2.f - backgroundBox.getMidY());
                if (auto parent = this->getParent(); parent) {
                    auto const parentPos = parent->convertToNodeSpace(this->convertToWorldSpace(targetPos));
                    parent->addChild(m_scrollLayer, 10);
                    m_scrollLayer->setPosition(parentPos);
                }
            } else {
                m_scrollLayer->setAnchorPoint({0.5f, 0.f});
                auto const targetPos = ccp(this->getContentWidth() - 70.f, this->getContentHeight() / 2.f + backgroundBox.getMidY());
                if (auto parent = this->getParent(); parent) {
                    auto const parentPos = parent->convertToNodeSpace(this->convertToWorldSpace(targetPos));
                    parent->addChild(m_scrollLayer, 10);
                    m_scrollLayer->setPosition(parentPos);
                }
            }
        }

        void closeSelector(CCObject* sender) {
            if (m_scrollLayer) m_scrollLayer->removeFromParent();
            m_arrowButton->setRotation(90.f);
            m_arrowButton->setTarget(this, menu_selector(ComboComponentNode::openSelector));
            s_activeCombo = nullptr;
        }

        void selectItem(CCObject* sender) {
            auto index = sender->getTag();
            if (index < 0 || index >= m_component->getItems().size()) return;
            m_component->setValue(index);
            m_component->triggerCallback(index);

            for (size_t i = 0; auto label : m_labels) {
                if (i == index) {
                    label->setColor(ThemeManager::get()->getButtonForegroundColor().toCCColor3B());
                } else {
                    label->setColor(ThemeManager::get()->getButtonDisabledForeground().toCCColor3B());
                }
                ++i;
            }

            this->updateLabel();
            this->closeSelector(nullptr);
        }

        bool init(float width) {
            if (!CCMenu::init()) return false;
            auto const tm = ThemeManager::get();

            this->setID(fmt::format("combo-{}"_spr, m_component->getId()));
            this->setContentSize({ width, 28.f });

            auto labelSize = (width * 0.6f) - 35.f;

            if (!m_component->getDescription().empty()) {
                auto spr = cocos2d::CCSprite::createWithSpriteFrameName("info.png"_spr);
                spr->setScale(0.35f);
                m_infoButton = CCMenuItemSpriteExtra::create(
                    spr, this,
                    menu_selector(BaseComponentNode::openDescriptionPopup)
                );
                m_infoButton->setAnchorPoint({ 0.5, 0.5f });
                m_infoButton->setColor(tm->getCheckboxCheckmarkColor().toCCColor3B());
                this->addChildAtPosition(m_infoButton, geode::Anchor::Right, { -10.f, 0.f });
                labelSize -= 15.f;
            }

            m_label = TranslatedLabel::create(m_component->getTitle());
            m_label->setColor(tm->getForegroundColor().toCCColor3B());
            m_label->setAnchorPoint({0, 0.5f});
            m_label->limitLabelWidth(labelSize, 1.f, 0.25f);
            this->addChildAtPosition(m_label, geode::Anchor::Left, { 15.f, 0.f });

            m_background = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
            m_background->setID("background");
            m_background->setAnchorPoint({ 0.5f, 0.5f });
            m_background->setScale(0.3f);
            m_background->setContentSize({365.f, 80.f});
            m_background->setColor(tm->getButtonBackgroundColor().toCCColor3B());
            m_background->setZOrder(-1);
            this->addChildAtPosition(m_background, geode::Anchor::Right, { -70.f, 0.f });

            auto spr = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
            spr->setScale(0.6f);

            m_arrowButton = CCMenuItemSpriteExtra::create(spr, this, menu_selector(ComboComponentNode::openSelector));
            m_arrowButton->setRotation(90.f);
            if (m_component->getItems().size() == 0) {
                m_arrowButton->setColor(tm->getButtonDisabledForeground().toCCColor3B());
                m_arrowButton->setEnabled(false);
            }
            this->addChildAtPosition(m_arrowButton, geode::Anchor::Right, { -25.f, 0.f });

            int index = m_component->getValue();
            if (index < 0 || index >= m_component->getItems().size()) {
                m_valueLabel = TranslatedLabel::createRaw("");
            } else {
                m_valueLabel = TranslatedLabel::createRaw(m_component->getItems().at(m_component->getValue()));
            }
            this->addChildAtPosition(m_valueLabel, geode::Anchor::Right, { -80.f, 0.f });
            updateLabel();

            return true;
        }
    };
}
