#pragma once

namespace eclipse::gui::cocos {

    /// @brief Base class for all component nodes.
    /// @tparam S The subclass type.
    /// @tparam T Cocos2d class to inherit from.
    /// @tparam U The component type.
    /// @tparam Args The constructor arguments.
    template<typename S, typename T, typename U, typename... Args>
    class BaseComponentNode : public T {
    protected:
        std::shared_ptr<U> m_component;
    public:
        virtual bool init(Args... args) = 0;

        static S* create(const std::shared_ptr<Component>& component, Args... args) {
            auto ret = new S;
            ret->m_component = std::static_pointer_cast<U>(component);
            if (ret->init(args...)) {
                ret->autorelease();
                return ret;
            }
            delete ret;
            return nullptr;
        }
    };


}