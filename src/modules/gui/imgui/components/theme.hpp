#pragma once
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <Geode/platform/platform.hpp>

namespace eclipse::gui {
    class Component;
    class LabelComponent;
    class ToggleComponent;
    class RadioButtonComponent;
    class ComboComponent;
    class FilesystemComboComponent;
    class SliderComponent;
    class InputFloatComponent;
    class InputIntComponent;
    class IntToggleComponent;
    class FloatToggleComponent;
    class InputTextComponent;
    class ColorComponent;
    class ButtonComponent;
    class KeybindComponent;
    class LabelSettingsComponent;
}

namespace eclipse::gui::imgui {

    constexpr float DEFAULT_SCALE = 1.0f GEODE_ANDROID(* 1.42f);
    constexpr float INV_DEFAULT_SCALE = 1.0f / DEFAULT_SCALE;

    extern std::vector<std::string> THEME_NAMES;
    enum class ComponentTheme {
        ImGui, /// Classic Dear ImGui look
        MegaHack, /// MegaHack v8 style
        MegaOverlay, /// GDMegaOverlay style
    };

    class Theme {
    public:
        virtual ~Theme() = default;

        /// @brief Accepts a component and uses current theme to render it
        void visit(const std::shared_ptr<Component>& component) const;

        /// @brief Renders the tooltip with correct sizing and theme color
        static void handleTooltip(const std::string& text);

        /// @brief Renders the context menu with "Add keybind" option
        /// @param id Keybind id
        static void handleKeybindMenu(std::string_view id);

        /// @brief Sets all default ImGui style properties
        virtual void init();
        /// @brief Runs global rescale every frame
        virtual void update();

        virtual bool beginWindow(const std::string& title);
        virtual void endWindow();

        // == Components == //

        virtual void visitLabel(const std::shared_ptr<LabelComponent>& label) const;
        virtual void visitToggle(const std::shared_ptr<ToggleComponent>& toggle) const;
        virtual void visitRadioButton(const std::shared_ptr<RadioButtonComponent>& radio) const;
        virtual void visitCombo(const std::shared_ptr<ComboComponent>& combo) const;
        virtual void visitFilesystemCombo(const std::shared_ptr<FilesystemComboComponent>& combo) const;
        virtual void visitSlider(const std::shared_ptr<SliderComponent>& slider) const;
        virtual void visitInputFloat(const std::shared_ptr<InputFloatComponent>& inputFloat) const;
        virtual void visitInputInt(const std::shared_ptr<InputIntComponent>& inputInt) const;
        virtual void visitIntToggle(const std::shared_ptr<IntToggleComponent>& intToggle) const;
        virtual void visitFloatToggle(const std::shared_ptr<FloatToggleComponent>& floatToggle) const;
        virtual void visitInputText(const std::shared_ptr<InputTextComponent>& inputText) const;
        virtual void visitColor(const std::shared_ptr<ColorComponent>& color) const;
        virtual void visitButton(const std::shared_ptr<ButtonComponent>& button) const;
        virtual void visitKeybind(const std::shared_ptr<KeybindComponent>& keybind) const;
        virtual void visitLabelSettings(const std::shared_ptr<LabelSettingsComponent>& labelSettings) const;

        // == ImGui Widgets == //

        virtual bool checkbox(
            const std::string& label, bool& value,
            bool isSearchedFor,
            const std::function<void()>& postDraw = []{}
        ) const;
        virtual bool checkboxWithSettings(
            const std::string& label, bool& value,
            bool isSearchedFor,
            const std::function<void()>& callback,
            const std::function<void()>& postDraw = []{},
            const std::string& popupId = "" // empty = use default
        ) const;
        virtual bool button(const std::string& text, bool isSearchedFor) const;

        virtual ComponentTheme getTheme() const { return ComponentTheme::ImGui; }
    };

}
