#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input float component to input a value from a range.
    class InputFloatComponent : public Component {
    public:
        explicit InputFloatComponent(
            std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f"
        );

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputFloatComponent* callback(const std::function<void(float)>& func);

        [[nodiscard]] const std::string& getId() const override;
        [[nodiscard]] const std::string& getTitle() const override;

        [[nodiscard]] float getMin() const;
        [[nodiscard]] float getMax() const;
        [[nodiscard]] const std::string& getFormat() const;

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;

        InputFloatComponent* setDescription(std::string description) override;

        InputFloatComponent* setDescription();

        void triggerCallback(float value) const;

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        float m_min;
        float m_max;
        std::function<void(float)> m_callback;
    };
}
