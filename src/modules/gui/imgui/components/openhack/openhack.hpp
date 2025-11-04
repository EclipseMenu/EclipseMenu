#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {
    class OpenHack final : public Theme {
        bool checkbox(
            std::string const& label, bool& value, bool isSearchedFor,
            FunctionRef<void()> postDraw
        ) const override;
        bool checkboxWithSettings(
            std::string const& label, bool& value, bool isSearchedFor,
            FunctionRef<void()> callback,
            FunctionRef<void()> postDraw,
            std::string const& popupId
        ) const override;
        bool button(std::string const& text, bool isSearchedFor) const override;
        void init() const override;

        void visitInputText(std::shared_ptr<InputTextComponent> const& inputText) const override;
        void visitInputFloat(std::shared_ptr<InputFloatComponent> const& inputFloat) const override;
        void visitInputInt(std::shared_ptr<InputIntComponent> const& inputInt) const override;
        void visitFloatToggle(std::shared_ptr<FloatToggleComponent> const& floatToggle) const override;
        void visitIntToggle(std::shared_ptr<IntToggleComponent> const& intToggle) const override;

        ComponentTheme getTheme() const override { return ComponentTheme::OpenHack; }
    };

    inline static constexpr OpenHack OPENHACK_THEME;
}
