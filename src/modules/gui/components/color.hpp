#pragma once
#include <functional>
#include <modules/gui/color.hpp>
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Input text component to get user input as a string.
    class ColorComponent : public Component {
    public:
        explicit ColorComponent(std::string title, std::string id, bool hasOpacity = false);

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ColorComponent* callback(Function<void(Color)>&& func);

        [[nodiscard]] std::string const& getId() const override;

        [[nodiscard]] std::string const& getTitle() const override;

        [[nodiscard]] bool hasOpacity() const;

        [[nodiscard]] Color getValue() const;
        void setValue(Color const& value) const;

        ColorComponent* setDescription(std::string description) override;

        void triggerCallback(Color const& value);

    private:
        std::string m_id;
        std::string m_title;
        bool m_hasOpacity;
        Function<void(Color)> m_callback;
    };
}
