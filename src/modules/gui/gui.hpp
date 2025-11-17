#pragma once

#include <functional.hpp>
#include <memory>
#include <string>
#include <vector>
#include "popup.hpp"
#include "components/base-component.hpp"

namespace eclipse::labels {
    struct LabelSettings;
}

namespace eclipse::gui {

    class MenuTab;
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

    using ComponentPtr = std::unique_ptr<Component>;

    /// @brief Contains a list of components and a title, to be passed into render engine.
    class MenuTab {
    public:
        MenuTab(std::string title, bool isSearchedFor);
        MenuTab(MenuTab&&) = default;
        MenuTab& operator=(MenuTab&&) = default;

        /// @brief Add a component to the tab.
        Component* addComponent(ComponentPtr component);

        /// @brief Remove a component from the tab.
        void removeComponent(Component* component);

        /// @brief Add a label to the tab.
        LabelComponent* addLabel(std::string title);

        /// @brief Add a checkbox to the tab. (id is the same as title)
        ToggleComponent* addToggle(std::string id);

        /// @brief Add a checkbox to the tab.
        ToggleComponent* addToggle(std::string title, std::string id);

        /// @brief Add a radio button to the tab.
        RadioButtonComponent* addRadioButton(std::string title, std::string id, int value);

        /// @brief Add a combo to the tab.
        ComboComponent* addCombo(std::string title, std::string id, std::vector<std::string> items, int value);

        /// @brief Add a combo to the tab. (id is the same as title)
        ComboComponent* addCombo(std::string id, std::vector<std::string> items, int value);

        /// @brief Add a filesystem combo button to the tab.
        FilesystemComboComponent* addFilesystemCombo(std::string title, std::string id, std::filesystem::path directory);

        /// @brief Add a filesystem combo button to the tab. (id is the same as title)
        FilesystemComboComponent* addFilesystemCombo(std::string id, std::filesystem::path directory);

        /// @brief Add a slider to the tab.
        SliderComponent* addSlider(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f");

        /// @brief Add a slider to the tab. (id is the same as title)
        SliderComponent* addSlider(std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f");

        /// @brief Add an input float to the tab.
        InputFloatComponent* addInputFloat(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f");

        /// @brief Add an input float to the tab.
        InputFloatComponent* addInputFloat(std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f");

        /// @brief Add an input int to the tab.
        InputIntComponent* addInputInt(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an input int to the tab.
        InputIntComponent* addInputInt(std::string id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an float toggle to the tab.
        IntToggleComponent* addIntToggle(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an float toggle to the tab. (id is the same as title)
        IntToggleComponent* addIntToggle(std::string id, int min = INT_MIN, int max = INT_MAX);

        /// @brief Add an float toggle to the tab.
        FloatToggleComponent* addFloatToggle(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f");

        /// @brief Add an float toggle to the tab. (id is the same as title)
        FloatToggleComponent* addFloatToggle(std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f");

        /// @brief Add an input text to the tab.
        InputTextComponent* addInputText(std::string title, std::string id);

        /// @brief Add an input text to the tab. (id is the same as title)
        InputTextComponent* addInputText(std::string id);

        /// @brief Add a button to the tab.
        ButtonComponent* addButton(std::string title);

        /// @brief Add a color picker to the tab.
        ColorComponent* addColorComponent(std::string title, std::string id, bool hasOpacity = false);

        /// @brief Add a color picker to the tab. (id is the same as title)
        ColorComponent* addColorComponent(std::string id, bool hasOpacity = false);

        /// @brief Add a keybind to the tab.
        KeybindComponent* addKeybind(std::string title, std::string id, bool canDelete = false);

        /// @brief Add a label settings to the tab.
        LabelSettingsComponent* addLabelSetting(labels::LabelSettings* settings);

        /// @brief Get the tab's title.
        [[nodiscard]] std::string const& getTitle() const { return m_title; }

        /// @brief Get the tab's components.
        [[nodiscard]] std::vector<ComponentPtr> const& getComponents() const { return m_components; }

        /// @brief Find a tab by name (or create a new one if it does not exist).
        /// (returns a pointer for legacy reasons)
        static MenuTab* find(std::string_view name);

        /// @brief Whether the tab is being searched for or not
        bool isSearchedFor() const { return m_isSearchedFor; }

        /// @brief Sets the tab's search state
        void setSearchedFor(bool state) { m_isSearchedFor = state; }

    private:
        std::string m_title;
        bool m_isSearchedFor;
        std::vector<ComponentPtr> m_components;
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
        virtual void queueAfterDrawing(Function<void()>&& func) = 0;

        /// @brief Opens a modal popup with provided configuration.
        virtual void showPopup(Popup&& popup) = 0;

        /// @brief Get the renderer type.
        [[nodiscard]] virtual RendererType getType() const = 0;

        /// @brief Triggered whenever a new tab is added.
        virtual void updateTabs() {}
    };

    using Tabs = std::vector<MenuTab>;

    /// @brief Main controller for the UI.
    class Engine {
    public:
        static Engine& get();

        Engine(Engine const&) = delete;
        void operator=(Engine const&) = delete;
        Engine(Engine&&) = delete;
        void operator=(Engine&&) = delete;

        void init();

        void toggle() const;

        void setRenderer(RendererType type);

        [[nodiscard]] Renderer* getRenderer() const { return m_renderer.get(); }
        [[nodiscard]] static RendererType getRendererType();

        /// @brief Check if the UI is visible.
        [[nodiscard]] bool isToggled() const {
            if (!m_renderer) return false;
            return m_renderer->isToggled();
        }

        /// @brief Find a tab by name.
        MenuTab& findTab(std::string_view name);

        /// @brief Calls the function after the main render loop
        template <typename Func>
        static void queueAfterDrawing(Func&& func) {
            if (auto renderer = get().m_renderer.get()) {
                // if cocos renderer, just call the function
                if (renderer->getType() == RendererType::Cocos2d) {
                    func();
                } else {
                    renderer->queueAfterDrawing(std::forward<Func>(func));
                }
            } else {
                func(); // fallback
            }
        }

        void showPopup(Popup&& popup) const {
            if (m_renderer) m_renderer->showPopup(std::move(popup));
        }

        [[nodiscard]] Tabs const& getTabs() const { return m_tabs; }
        [[nodiscard]] Tabs& getTabs() { return m_tabs; }
        [[nodiscard]] bool isInitialized() const { return m_initialized; }

    private:
        Engine();

        std::unique_ptr<Renderer> m_renderer;
        Tabs m_tabs;
        bool m_initialized = false;
    };

}
