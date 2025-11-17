#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input text component to get user input as a string.
    class InputTextComponent : public Component {
    public:
        explicit InputTextComponent(std::string title, std::string id);

        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputTextComponent* callback(Function<void(std::string)>&& func);

        [[nodiscard]] std::string const& getId() const override;
        [[nodiscard]] std::string const& getTitle() const override;

        [[nodiscard]] std::string getValue() const;
        void setValue(std::string value) const;

        InputTextComponent* setDescription(std::string description) override;

        void triggerCallback(std::string value);

    private:
        std::string m_id;
        std::string m_title;
        Function<void(std::string)> m_callback;
    };
}
