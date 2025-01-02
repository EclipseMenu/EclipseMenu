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
        FloatToggleComponent* toggleCallback(const std::function<void()>& func);
        FloatToggleComponent* valueCallback(const std::function<void(float)>& func);

        /// @brief Set toggle description.
        FloatToggleComponent* setDescription(std::string description) override;
        FloatToggleComponent* setDescription();

        /// @brief Allows to set keybinds for the toggle.
        FloatToggleComponent* handleKeybinds();

        [[nodiscard]] const std::string& getId() const override;
        [[nodiscard]] const std::string& getTitle() const override;

        [[nodiscard]] bool hasKeybind() const;

        [[nodiscard]] float getMin() const;
        [[nodiscard]] float getMax() const;

        [[nodiscard]] const std::string& getFormat() const;

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;

        [[nodiscard]] bool getState() const;
        void setState(bool value) const;

        void triggerCallback(float value) const;
        void triggerCallback() const;

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        std::string m_description;
        float m_min;
        float m_max;
        std::function<void(float)> m_valueCallback;
        std::function<void()> m_toggleCallback;
        bool m_hasKeybind = false;
    };
}
