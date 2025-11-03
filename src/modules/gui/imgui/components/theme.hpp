#pragma once
#include <memory>
#include <string>
#include <vector>
#include <std23/function_ref.h>
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
        void visit(std::shared_ptr<Component> const& component) const;

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

        virtual void visitLabel(std::shared_ptr<LabelComponent> const& label) const;
        virtual void visitToggle(std::shared_ptr<ToggleComponent> const& toggle) const;
        virtual void visitRadioButton(std::shared_ptr<RadioButtonComponent> const& radio) const;
        virtual void visitCombo(std::shared_ptr<ComboComponent> const& combo) const;
        virtual void visitFilesystemCombo(std::shared_ptr<FilesystemComboComponent> const& combo) const;
        virtual void visitSlider(std::shared_ptr<SliderComponent> const& slider) const;
        virtual void visitInputFloat(std::shared_ptr<InputFloatComponent> const& inputFloat) const;
        virtual void visitInputInt(std::shared_ptr<InputIntComponent> const& inputInt) const;
        virtual void visitIntToggle(std::shared_ptr<IntToggleComponent> const& intToggle) const;
        virtual void visitFloatToggle(std::shared_ptr<FloatToggleComponent> const& floatToggle) const;
        virtual void visitInputText(std::shared_ptr<InputTextComponent> const& inputText) const;
        virtual void visitColor(std::shared_ptr<ColorComponent> const& color) const;
        virtual void visitButton(std::shared_ptr<ButtonComponent> const& button) const;
        virtual void visitKeybind(std::shared_ptr<KeybindComponent> const& keybind) const;
        virtual void visitLabelSettings(std::shared_ptr<LabelSettingsComponent> const& labelSettings) const;

        // == ImGui Widgets == //

        virtual bool checkbox(
            std::string const& label, bool& value,
            bool isSearchedFor,
            std23::function_ref<void()> postDraw = [] {}
        ) const;
        virtual bool checkboxWithSettings(
            std::string const& label, bool& value,
            bool isSearchedFor,
            std23::function_ref<void()> callback,
            std23::function_ref<void()> postDraw = [] {},
            std::string const& popupId = "" // empty = use default
        ) const;
        virtual bool button(std::string const& text, bool isSearchedFor) const;

        virtual ComponentTheme getTheme() const { return ComponentTheme::ImGui; }
    };

    namespace themes {
        inline static constexpr Theme DEFAULT_THEME;
    }
}
