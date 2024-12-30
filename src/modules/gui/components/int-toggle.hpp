#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input int component to input a value from a range. Can be toggled.
    class IntToggleComponent : public Component {
    public:
        explicit IntToggleComponent(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        IntToggleComponent* toggleCallback(const std::function<void()>& func);

        IntToggleComponent* valueCallback(const std::function<void(int)>& func);

        /// @brief Set toggle description.
        IntToggleComponent* setDescription(std::string description) override;

        /// @brief Allows to set keybinds for the toggle.
        IntToggleComponent* handleKeybinds();

        [[nodiscard]] const std::string& getId() const override;
        [[nodiscard]] const std::string& getTitle() const override;
        [[nodiscard]] bool hasKeybind() const;

        [[nodiscard]] int getMin() const;
        [[nodiscard]] int getMax() const;

        [[nodiscard]] int getValue() const;
        void setValue(int value) const;
        [[nodiscard]] bool getState() const;
        void setState(bool value) const;

        void triggerCallback(int value) const;
        void triggerCallback() const;

    private:
        std::string m_id;
        std::string m_title;
        std::string m_description;
        int m_min;
        int m_max;
        std::function<void(int)> m_valueCallback;
        std::function<void()> m_toggleCallback;
        bool m_hasKeybind = false;
    };
}
