#pragma once
#include <functional>
#include <modules/keybinds/manager.hpp>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Component for picking a keybind.
    class KeybindComponent : public Component {
    public:
        explicit KeybindComponent(std::string title, std::string id, bool canDelete = false);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        KeybindComponent* callback(const std::function<void(keybinds::Keys)>& func);

        /// @brief Sets the default callback for internal keybinds.
        KeybindComponent* setInternal();
        /// @brief Sets the default key for the keybind.
        KeybindComponent* setDefaultKey(keybinds::Keys key);

        [[nodiscard]] const std::string& getId() const override;
        [[nodiscard]] const std::string& getTitle() const override;
        [[nodiscard]] bool canDelete() const;
        [[nodiscard]] keybinds::Keys getDefaultKey() const;

        KeybindComponent* setDescription(std::string description) override;

        void triggerCallback(keybinds::Keys key) const;

    private:
        std::string m_id;
        std::string m_title;
        keybinds::Keys m_defaultKey = keybinds::Keys::None;
        bool m_canDelete;
        std::function<void(keybinds::Keys)> m_callback;
    };
}
