#include "oneof-picker.hpp"

#include <modules/gui/theming/manager.hpp>

#include "FallbackBMFont.hpp"

namespace eclipse::gui::cocos {
    OneOfPicker* OneOfPicker::create(std::vector<std::string>&& options, Function<void(int)>&& callback, size_t index) {
        auto ret = new OneOfPicker();
        if (ret->init(std::move(options), std::move(callback), index)) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    OneOfPicker* OneOfPicker::create(std::span<char const*> options, Function<void(int)>&& callback, size_t index) {
        std::vector<std::string> items;
        items.reserve(options.size());
        for (auto& option : options) {
            items.emplace_back(option);
        }
        return create(std::move(items), std::move(callback), index);
    }

    void OneOfPicker::setSelected(size_t index) {
        if (index >= m_options.size()) {
            return;
        }
        m_selected = index;
        this->updateValueLabel();
    }

    void OneOfPicker::setItems(std::vector<std::string>&& items) {
        if (m_selected >= items.size()) {
            m_selected = 0;
        }
        m_options = std::move(items);
        this->updateValueLabel();
    }

    bool OneOfPicker::init(std::vector<std::string>&& options, Function<void(int)>&& callback, size_t index) {
        if (!CCMenu::init()) return false;

        m_options = std::move(options);
        m_callback = std::move(callback);
        m_selected = index;

        auto const tm = ThemeManager::get();

        this->setContentSize({100.f, 28.f});
        this->setID("oneof-picker"_spr);

        m_background = cocos2d::extension::CCScale9Sprite::create("square02b_001.png");
        m_background->setID("background");
        m_background->setScale(0.3f);
        m_background->setColor(tm->getButtonBackgroundColor().toCCColor3B());
        m_background->setZOrder(-1);
        m_background->setContentSize(this->getContentSize() / 0.3f);
        this->addChildAtPosition(m_background, geode::Anchor::Center);

        auto spr = cocos2d::CCSprite::createWithSpriteFrameName("edit_rightBtn_001.png");
        spr->setScale(0.6f);
        auto arrowBtn = CCMenuItemSpriteExtra::create(spr, this, menu_selector(OneOfPicker::onArrowClick));
        arrowBtn->setTag(1);
        this->addChildAtPosition(arrowBtn, geode::Anchor::Right, { -8.f, 0.f });

        auto arrowBtn2 = CCMenuItemSpriteExtra::create(spr, this, menu_selector(OneOfPicker::onArrowClick));
        arrowBtn2->setRotation(180.f);
        arrowBtn2->setTag(-1);
        this->addChildAtPosition(arrowBtn2, geode::Anchor::Left, { 8.f, 0.f });

        m_valueLabel = TranslatedLabel::createRaw("");
        this->addChildAtPosition(m_valueLabel, geode::Anchor::Center);

        this->updateValueLabel();

        return true;
    }

    void OneOfPicker::onArrowClick(CCObject* sender) {
        if (m_options.empty()) return;

        int tag = sender->getTag();
        int value = m_selected + tag;
        if (value < 0) value = std::max<int>(m_options.size() - 1, 0);
        if (value >= m_options.size()) value = 0;

        m_selected = value;
        this->updateValueLabel();

        m_callback(value);
    }

    void OneOfPicker::updateValueLabel() {
        if (m_options.empty()) {
            m_valueLabel->setString("");
        } else if (m_selected >= m_options.size()) {
            m_selected = 0;
        }

        m_valueLabel->setString(m_options[m_selected]);
        m_valueLabel->limitLabelWidth(getContentWidth() * 0.6f, 1.f, 0.25f);
    }
}
