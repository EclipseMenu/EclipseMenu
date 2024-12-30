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
        ColorComponent* callback(const std::function<void(Color)>& func);

        [[nodiscard]] const std::string& getId() const override;

        [[nodiscard]] const std::string& getTitle() const override;

        [[nodiscard]] bool hasOpacity() const;

        [[nodiscard]] Color getValue() const;
        void setValue(const Color& value) const;

        ColorComponent* setDescription(std::string description) override;

        void triggerCallback(const Color& value) const;

    private:
        std::string m_id;
        std::string m_title;
        bool m_hasOpacity;
        std::function<void(Color)> m_callback;
    };
}
