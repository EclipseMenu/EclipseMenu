#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Slider component to select a value from a range.
    class SliderComponent : public Component {
    public:
        explicit SliderComponent(
            std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f"
        );

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        SliderComponent* callback(std::function<void(float)>&& func);

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;

        [[nodiscard]] float getMin() const;
        [[nodiscard]] float getMax() const;
        [[nodiscard]] std::string const& getFormat() const;

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;

        SliderComponent* setDescription(std::string description) override;

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
