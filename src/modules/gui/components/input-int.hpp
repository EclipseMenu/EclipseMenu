#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input int component to input a value from a range.
    class InputIntComponent : public Component {
    public:
        explicit InputIntComponent(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputIntComponent* callback(std::function<void(int)>&& func);

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;

        [[nodiscard]] int getMin() const;
        [[nodiscard]] int getMax() const;

        [[nodiscard]] int getValue() const;
        void setValue(int value) const;

        InputIntComponent* setDescription(std::string description) override;

        void triggerCallback(int value) const;

    private:
        std::string m_id;
        std::string m_title;
        int m_min;
        int m_max;
        std::function<void(int)> m_callback;
    };
}
