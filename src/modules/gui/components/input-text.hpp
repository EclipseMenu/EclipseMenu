#pragma once
#include <functional>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input text component to get user input as a string.
    class InputTextComponent : public Component {
    public:
        explicit InputTextComponent(std::string title, std::string id);

        void onInit() override {};
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputTextComponent* callback(const std::function<void(std::string)>& func);

        [[nodiscard]] const std::string& getId() const override;
        [[nodiscard]] const std::string& getTitle() const override;

        [[nodiscard]] std::string getValue() const;
        void setValue(const std::string& value) const;

        InputTextComponent* setDescription(std::string description) override;

        void triggerCallback(std::string value) const;

    private:
        std::string m_id;
        std::string m_title;
        std::function<void(std::string)> m_callback;
    };
}
