#pragma once
#include <memory>
#include <string>
#include <vector>

#include <functional.hpp>
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
        ImGui,       /// Classic Dear ImGui look
        MegaHack,    /// MegaHack v8 style
        MegaOverlay, /// GDMegaOverlay style
        Gruvbox,     /// Gruvbox style from OpenHack
        OpenHack,    /// Classic OpenHack style
    };

    class Theme {
    public:
        constexpr Theme() = default;
        Theme(Theme const&) = delete;
        Theme& operator=(Theme const&) = delete;
        Theme(Theme&&) = delete;
        Theme& operator=(Theme&&) = delete;

        /// @brief Accepts a component and uses current theme to render it
        void visit(Component* component) const;

        /// @brief Renders the tooltip with correct sizing and theme color
        static void handleTooltip(std::string const& text);

        /// @brief Renders the context menu with "Add keybind" option
        /// @param id Keybind id
        static void handleKeybindMenu(std::string_view id);

        /// @brief Sets all default ImGui style properties
        virtual void init() const;
        /// @brief Runs global rescale every frame
        virtual void update() const;

        virtual bool beginWindow(std::string const& title) const;
        virtual void endWindow() const;

        // == Components == //

        virtual void visitLabel(LabelComponent* label) const;
        virtual void visitToggle(ToggleComponent* toggle) const;
        virtual void visitRadioButton(RadioButtonComponent* radio) const;
        virtual void visitCombo(ComboComponent* combo) const;
        virtual void visitFilesystemCombo(FilesystemComboComponent* combo) const;
        virtual void visitSlider(SliderComponent* slider) const;
        virtual void visitInputFloat(InputFloatComponent* inputFloat) const;
        virtual void visitInputInt(InputIntComponent* inputInt) const;
        virtual void visitIntToggle(IntToggleComponent* intToggle) const;
        virtual void visitFloatToggle(FloatToggleComponent* floatToggle) const;
        virtual void visitInputText(InputTextComponent* inputText) const;
        virtual void visitColor(ColorComponent* color) const;
        virtual void visitButton(ButtonComponent* button) const;
        virtual void visitKeybind(KeybindComponent* keybind) const;
        virtual void visitLabelSettings(LabelSettingsComponent* labelSettings) const;

        // == ImGui Widgets == //

        virtual bool checkbox(
            std::string const& label, bool& value,
            bool isSearchedFor,
            FunctionRef<void()> postDraw = [] {}
        ) const;
        virtual bool checkboxWithSettings(
            std::string const& label, bool& value,
            bool isSearchedFor,
            FunctionRef<void()> callback,
            FunctionRef<void()> postDraw = [] {},
            std::string const& popupId = "" // empty = use default
        ) const;
        virtual bool button(std::string const& text, bool isSearchedFor) const;

        virtual ComponentTheme getTheme() const { return ComponentTheme::ImGui; }
    };

    namespace themes {
        inline static constexpr Theme DEFAULT_THEME;
    }
}
