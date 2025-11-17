#pragma once
#include <functional>
#include <vector>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Combo component for selecting one of the options.
    class ComboComponent : public Component {
    public:
        explicit ComboComponent(std::string id, std::string title, std::vector<std::string> items, int value);

        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ComboComponent* callback(Function<void(int)>&& func);

        /// @brief Get the combo value (selected item index).
        [[nodiscard]] int getValue() const;

        /// @brief Get the combo items.
        [[nodiscard]] std::vector<std::string> const& getItems() const;

        void setItems(std::vector<std::string> const& items);

        /// @brief Set the combo value (selected item index).
        void setValue(int value) const;

        /// @brief Set the combo value if empty (selected item index).
        void setValueIfEmpty(int value) const;

        [[nodiscard]] std::string const& getId() const override;

        [[nodiscard]] std::string const& getTitle() const override;

        ComboComponent* setDescription(std::string description) override;

        ComboComponent* setDescription();

        void triggerCallback(int value);

    private:
        std::string m_id;
        std::string m_title;
        int m_value;
        std::vector<std::string> m_items;
        Function<void(int)> m_callback;
    };
}
