#pragma once
#include <functional.hpp>
#include "base-component.hpp"
#include "../gui.hpp"

namespace eclipse::gui {
    class MenuTab;

    /// @brief Simple toggle component, that displays a title and a checkbox.
    class ToggleComponent : public Component {
    public:
        explicit ToggleComponent(std::string id, std::string title);

        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ToggleComponent* callback(Function<void(bool)>&& func);

        /// @brief Add sub-component to toggle.
        void addOptions(FunctionRef<void(MenuTab*)> options);

        /// @brief Get the toggle value.
        [[nodiscard]] bool getValue() const;

        /// @brief Set the toggle value.
        void setValue(bool value);

        /// @brief Allows to set keybinds for the toggle.
        ToggleComponent* handleKeybinds();

        ToggleComponent* setDescription(std::string description) override;
        ToggleComponent* setDescription();

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;
        [[nodiscard]] MenuTab* getOptions() const;
        [[nodiscard]] bool hasKeybind() const;

        void triggerCallback(bool value);

    private:
        std::string m_id;
        std::string m_title;
        Function<void(bool)> m_callback;
        std::unique_ptr<MenuTab> m_options = nullptr;
        bool m_hasKeybind = false;
    };
}
