#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    class MenuTab;

    /// @brief Simple toggle component, that displays a title and a checkbox.
    class ToggleComponent : public Component {
    public:
        explicit ToggleComponent(std::string id, std::string title);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ToggleComponent* callback(const std::function<void(bool)>& func);

        /// @brief Add sub-component to toggle.
        void addOptions(const std::function<void(std::shared_ptr<MenuTab>)>& options);

        /// @brief Get the toggle value.
        [[nodiscard]] bool getValue() const;

        /// @brief Set the toggle value.
        void setValue(bool value) const;

        /// @brief Allows to set keybinds for the toggle.
        ToggleComponent* handleKeybinds();

        ToggleComponent* setDescription(std::string description) override;
        ToggleComponent* setDescription();

        [[nodiscard]] const std::string& getId() const override;
        [[nodiscard]] const std::string& getTitle() const override;
        [[nodiscard]] std::weak_ptr<MenuTab> getOptions() const;
        [[nodiscard]] bool hasKeybind() const;

        void triggerCallback(bool value) const;

    private:
        std::string m_id;
        std::string m_title;
        std::function<void(bool)> m_callback;
        std::shared_ptr<MenuTab> m_options = nullptr;
        bool m_hasKeybind = false;
    };
}
