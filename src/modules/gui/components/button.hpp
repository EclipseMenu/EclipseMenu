#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Button component to execute an action when pressed.
    class ButtonComponent : public Component {
    public:
        explicit ButtonComponent(std::string title);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ButtonComponent* callback(const std::function<void()>& func);

        [[nodiscard]] const std::string& getId() const override;

        [[nodiscard]] const std::string& getTitle() const override;

        ButtonComponent* setDescription(std::string description) override;

        ButtonComponent* setDescription();

        /// @brief Allows to set keybinds for the button.
        ButtonComponent* handleKeybinds();

        [[nodiscard]] bool hasKeybind() const;

        void triggerCallback() const;

    private:
        std::string m_title;
        std::function<void()> m_callback;
        bool m_hasKeybind = false;
    };
}
