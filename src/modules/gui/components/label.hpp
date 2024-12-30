#pragma once
#include "base-component.hpp"

namespace eclipse::gui {
    /// @brief Simple label component, that displays a title.
    class LabelComponent : public Component {
    public:
        explicit LabelComponent(std::string title);

        void onInit() override {}
        void onUpdate() override {}

        void setText(std::string text);

        [[nodiscard]] const std::string& getId() const override;

        [[nodiscard]] const std::string& getTitle() const override;

        LabelComponent* setDescription(std::string description) override;

    private:
        std::string m_title;
    };
}
