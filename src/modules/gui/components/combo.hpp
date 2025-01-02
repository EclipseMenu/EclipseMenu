#pragma once
#include <functional>
#include <vector>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Combo component for selecting one of the options.
    class ComboComponent : public Component {
    public:
        explicit ComboComponent(std::string id, std::string title, std::vector<std::string> items, int value);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ComboComponent* callback(const std::function<void(int)>& func);

        /// @brief Get the combo value (selected item index).
        [[nodiscard]] int getValue() const;

        /// @brief Get the combo items.
        [[nodiscard]] const std::vector<std::string>& getItems() const;

        void setItems(const std::vector<std::string>& items);

        /// @brief Set the combo value (selected item index).
        void setValue(int value) const;

        /// @brief Set the combo value if empty (selected item index).
        void setValueIfEmpty(int value) const;

        [[nodiscard]] const std::string& getId() const override;

        [[nodiscard]] const std::string& getTitle() const override;

        ComboComponent* setDescription(std::string description) override;

        ComboComponent* setDescription();

        void triggerCallback(int value) const;

    private:
        std::string m_id;
        std::string m_title;
        int m_value;
        std::vector<std::string> m_items;
        std::function<void(int)> m_callback;
    };
}
