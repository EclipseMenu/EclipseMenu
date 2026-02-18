#pragma once
#include <functional>
#include <modules/keybinds/manager.hpp>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Component for picking a keybind.
    class KeybindComponent : public Component {
    public:
        explicit KeybindComponent(std::string title, std::string id, bool canDelete = false);

        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        KeybindComponent* callback(Function<void(keybinds::KeybindProps)>&& func);

        /// @brief Sets the default callback for internal keybinds.
        KeybindComponent* setInternal();
        /// @brief Sets the default key for the keybind.
        KeybindComponent* setDefaultKey(keybinds::KeybindProps key);

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;
        [[nodiscard]] bool canDelete() const;
        [[nodiscard]] keybinds::KeybindProps getDefaultKey() const;

        KeybindComponent* setDescription(std::string description) override;

        void triggerCallback(keybinds::KeybindProps key);

    private:
        std::string m_id;
        std::string m_title;
        keybinds::KeybindProps m_defaultKey = {keybinds::Keys::None, geode::KeyboardModifier::None};
        bool m_canDelete;
        Function<void(keybinds::KeybindProps)> m_callback;
    };
}
