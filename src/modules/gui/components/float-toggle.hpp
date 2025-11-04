#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input float component to input a value from a range. Can be toggled .
    class FloatToggleComponent : public Component {
    public:
        explicit FloatToggleComponent(
            std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f"
        );

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        FloatToggleComponent* toggleCallback(Function<void()>&& func);
        FloatToggleComponent* valueCallback(Function<void(float)>&& func);

        /// @brief Set toggle description.
        FloatToggleComponent* setDescription(std::string description) override;
        FloatToggleComponent* setDescription();

        /// @brief Allows to set keybinds for the toggle.
        FloatToggleComponent* handleKeybinds();

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;

        [[nodiscard]] bool hasKeybind() const;

        [[nodiscard]] float getMin() const;
        [[nodiscard]] float getMax() const;

        [[nodiscard]] std::string const& getFormat() const;

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;

        [[nodiscard]] bool getState() const;
        void setState(bool value) const;

        void triggerCallback(float value);
        void triggerCallback();

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        std::string m_description;
        float m_min;
        float m_max;
        Function<void(float)> m_valueCallback;
        Function<void()> m_toggleCallback;
        bool m_hasKeybind = false;
    };
}
