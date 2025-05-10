#pragma once
#include <modules/gui/imgui/components/theme.hpp>

namespace eclipse::gui::imgui::themes {
    class OpenHack : public Theme {
        bool checkbox(
            const std::string& label, bool& value, bool isSearchedFor,
            const std::function<void()>& postDraw
        ) const override;
        bool checkboxWithSettings(
            const std::string& label, bool& value, bool isSearchedFor,
            const std::function<void()>& callback,
            const std::function<void()>& postDraw,
            const std::string& popupId
        ) const override;
        bool button(const std::string& text, bool isSearchedFor) const override;
        void init() override;

        void visitInputText(const std::shared_ptr<InputTextComponent>& inputText) const override;
        void visitInputFloat(const std::shared_ptr<InputFloatComponent>& inputFloat) const override;
        void visitInputInt(const std::shared_ptr<InputIntComponent>& inputInt) const override;
        void visitFloatToggle(const std::shared_ptr<FloatToggleComponent>& floatToggle) const override;
        void visitIntToggle(const std::shared_ptr<IntToggleComponent>& intToggle) const override;

        ComponentTheme getTheme() const override { return ComponentTheme::OpenHack; }
    };
}
