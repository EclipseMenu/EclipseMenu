#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input int component to input a value from a range. Can be toggled.
    class IntToggleComponent : public Component {
    public:
        explicit IntToggleComponent(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX);

        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        IntToggleComponent* toggleCallback(Function<void()>&& func);

        IntToggleComponent* valueCallback(Function<void(int)>&& func);

        /// @brief Set toggle description.
        IntToggleComponent* setDescription(std::string description) override;

        /// @brief Allows to set keybinds for the toggle.
        IntToggleComponent* handleKeybinds();

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;
        [[nodiscard]] bool hasKeybind() const;

        [[nodiscard]] int getMin() const;
        [[nodiscard]] int getMax() const;

        [[nodiscard]] int getValue() const;
        void setValue(int value) const;
        [[nodiscard]] bool getState() const;
        void setState(bool value) const;

        void triggerCallback(int value);
        void triggerCallback();

    private:
        std::string m_id;
        std::string m_title;
        std::string m_description;
        int m_min;
        int m_max;
        Function<void(int)> m_valueCallback;
        Function<void()> m_toggleCallback;
        bool m_hasKeybind = false;
    };
}
