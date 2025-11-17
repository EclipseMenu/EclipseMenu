#pragma once
#include <modules/gui/popup.hpp>
#include <modules/gui/cocos/nodes/FallbackBMFont.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/i18n/translations.hpp>

namespace eclipse::gui::cocos {

    /// @brief Base class for all component nodes.
    /// @tparam S The subclass type.
    /// @tparam T Cocos2d class to inherit from.
    /// @tparam U The component type.
    /// @tparam Args The constructor arguments.
    template<typename S, typename T, typename U, typename... Args>
    class BaseComponentNode : public T {
    protected:
        U* m_component{};
    public:
        static S* create(Component* component, Args... args) {
            auto ret = new S;
            ret->m_component = static_cast<U*>(component);
            if (ret->S::init(args...)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }

        void openDescriptionPopup(cocos2d::CCObject*) {
            eclipse::Popup::create(
                i18n::get_(m_component->getTitle()),
                i18n::get_(m_component->getDescription())
            );
        }

        static cocos2d::CCSprite* createButton(char const* innerFrameName, float scale = 0.4f) {
            auto const tm = ThemeManager::get();
            auto box = cocos2d::CCSprite::createWithSpriteFrameName("rectangle.png"_spr);
            box->setScale(scale);
            if (innerFrameName) {
                auto inner = cocos2d::CCSprite::createWithSpriteFrameName(innerFrameName);
                box->addChildAtPosition(inner, geode::Anchor::Center);
                inner->setColor(tm->getCheckboxCheckmarkColor().toCCColor3B());
            }
            box->setColor(tm->getCheckboxBackgroundColor().toCCColor3B());
            return box;
        }
    };


}