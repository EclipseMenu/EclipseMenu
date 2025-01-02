#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "popup.hpp"

namespace eclipse::labels {
    struct LabelSettings;
}

namespace eclipse::gui {

    class MenuTab;
    class Component;
    class ButtonComponent;
    class ColorComponent;
    class ComboComponent;
    class FilesystemComboComponent;
    class FloatToggleComponent;
    class InputFloatComponent;
    class InputIntComponent;
    class InputTextComponent;
    class IntToggleComponent;
    class KeybindComponent;
    class LabelComponent;
    class LabelSettingsComponent;
    class RadioButtonComponent;
    class SliderComponent;
    class ToggleComponent;

    /// @brief Contains a list of components and a title, to be passed into render engine.
    class MenuTab {
    public:
        explicit MenuTab(std::string title, bool isSearchedFor);

        /// @brief Add a component to the tab.
        void addComponent(const std::shared_ptr<Component>& component);

        /// @brief Remove a component from the tab.
        void removeComponent(std::weak_ptr<Component> component);

        /// @brief Add a label to the tab.
        std::shared_ptr<LabelComponent> addLabel(const std::string& title);

        /// @brief Add a checkbox to the tab. (id is the same as title)
        std::shared_ptr<ToggleComponent> addToggle(const std::string& id);

        /// @brief Add a checkbox to the tab.
        std::shared_ptr<ToggleComponent> addToggle(const std::string& title, const std::string& id);

        /// @brief Add a radio button to the tab.
        std::shared_ptr<RadioButtonComponent> addRadioButton(const std::string& title, const std::string& id, int value);

        /// @brief Add a combo to the tab.
        std::shared_ptr<ComboComponent> addCombo(const std::string& title, const std::string& id, std::vector<std::string> items, int value);

        /// @brief Add a combo to the tab. (id is the same as title)
        std::shared_ptr<ComboComponent> addCombo(const std::string& id, std::vector<std::string> items, int value);

        /// @brief Add a filesystem combo button to the tab.
        std::shared_ptr<FilesystemComboComponent> addFilesystemCombo(const std::string& title, const std::string& id, std::filesystem::path directory);

        /// @brief Add a filesystem combo button to the tab. (id is the same as title)
        std::shared_ptr<FilesystemComboComponent> addFilesystemCombo(const std::string& id, std::filesystem::path directory);

        /// @brief Add a slider to the tab.
        std::shared_ptr<SliderComponent> addSlider(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f");

        /// @brief Add a slider to the tab. (id is the same as title)
        std::shared_ptr<SliderComponent> addSlider(const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f");

        /// @brief Add an input float to the tab.
        std::shared_ptr<InputFloatComponent> addInputFloat(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f");

        /// @brief Add an input float to the tab.
        std::shared_ptr<InputFloatComponent> addInputFloat(const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f");

        /// @brief Add an input int to the tab.
        std::shared_ptr<InputIntComponent> addInputInt(const std::string& title, const std::string& id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an input int to the tab.
        std::shared_ptr<InputIntComponent> addInputInt(const std::string& id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an float toggle to the tab.
        std::shared_ptr<IntToggleComponent> addIntToggle(const std::string& title, const std::string& id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an float toggle to the tab. (id is the same as title)
        std::shared_ptr<IntToggleComponent> addIntToggle(const std::string& id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an float toggle to the tab.
        std::shared_ptr<FloatToggleComponent> addFloatToggle(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f");

        /// @brief Add an float toggle to the tab. (id is the same as title)
        std::shared_ptr<FloatToggleComponent> addFloatToggle(const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f");

        /// @brief Add an input text to the tab.
        std::shared_ptr<InputTextComponent> addInputText(const std::string& title, const std::string& id);

        /// @brief Add an input text to the tab. (id is the same as title)
        std::shared_ptr<InputTextComponent> addInputText(const std::string& id);

        /// @brief Add a button to the tab.
        std::shared_ptr<ButtonComponent> addButton(const std::string& title);

        /// @brief Add a color picker to the tab.
        std::shared_ptr<ColorComponent> addColorComponent(const std::string& title, const std::string& id, bool hasOpacity = false);

        /// @brief Add a color picker to the tab. (id is the same as title)
        std::shared_ptr<ColorComponent> addColorComponent(const std::string& id, bool hasOpacity = false);

        /// @brief Add a keybind to the tab.
        std::shared_ptr<KeybindComponent> addKeybind(const std::string& title, const std::string& id, bool canDelete = false);

        /// @brief Add a label settings to the tab.
        std::shared_ptr<LabelSettingsComponent> addLabelSetting(labels::LabelSettings* settings);

        /// @brief Get the tab's title.
        [[nodiscard]] const std::string& getTitle() const { return m_title; }

        /// @brief Get the tab's components.
        [[nodiscard]] const std::vector<std::shared_ptr<Component>>& getComponents() const { return m_components; }

        /// @brief Find a tab by name (or create a new one if it does not exist).
        static std::shared_ptr<MenuTab> find(std::string_view name);

        /// @brief Whether the tab is being searched for or not
        bool isSearchedFor() const { return m_isSearchedFor; };

        /// @brief Sets the tab's search state
        void setSearchedFor(bool state) { m_isSearchedFor = state; };

    private:
        std::string m_title;
        bool m_isSearchedFor;
        std::vector<std::shared_ptr<Component>> m_components;
    };

    enum class RendererType {
        None = -1,
        ImGui,
        Cocos2d
    };

    /// @brief Abstract class, that wraps all UI function calls.
    class Renderer {
    public:
        virtual ~Renderer() = default;

        /// @brief Initialize the renderer.
        virtual void init() = 0;

        /// @brief Toggle the UI visibility.
        virtual void toggle() = 0;

        /// @brief Tell the renderer to clean up/unload.
        virtual void shutdown() = 0;

        /// @brief Check if the UI is visible.
        [[nodiscard]] virtual bool isToggled() const = 0;

        /// @brief [Implementation specific] Calls the function after the main render loop
        virtual void queueAfterDrawing(const std::function<void()>& func) = 0;

        /// @brief Opens a modal popup with provided configuration.
        virtual void showPopup(const Popup& popup) = 0;

        /// @brief Get the renderer type.
        [[nodiscard]] virtual RendererType getType() const = 0;

        /// @brief Triggered whenever a new tab is added.
        virtual void updateTabs() {}
    };

    using Tabs = std::vector<std::shared_ptr<MenuTab>>;

    /// @brief Main controller for the UI.
    class Engine {
    public:
        static std::shared_ptr<Engine> get();

        void init();

        void toggle() const;

        void setRenderer(RendererType type);

        [[nodiscard]] std::shared_ptr<Renderer> getRenderer() const { return m_renderer; }
        [[nodiscard]] static RendererType getRendererType();

        /// @brief Check if the UI is visible.
        [[nodiscard]] bool isToggled() const {
            if (!m_renderer) return false;
            return m_renderer->isToggled();
        }

        /// @brief Find a tab by name.
        std::shared_ptr<MenuTab> findTab(std::string_view name);

        /// @brief Calls the function after the main render loop
        static void queueAfterDrawing(const std::function<void()>& func) {
            if (auto renderer = get()->m_renderer)
                renderer->queueAfterDrawing(func);
            else func(); // fallback
        }

        void showPopup(const Popup& popup) const {
            if (m_renderer) m_renderer->showPopup(popup);
        }

        [[nodiscard]] const Tabs& getTabs() const { return m_tabs; }
        [[nodiscard]] bool isInitialized() const { return m_initialized; }

    private:
        std::shared_ptr<Renderer> m_renderer;
        Tabs m_tabs;
        bool m_initialized = false;
    };

}
