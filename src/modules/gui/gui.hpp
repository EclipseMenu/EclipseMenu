#pragma once

#include <modules/keybinds/manager.hpp>
#include <modules/gui/color.hpp>

#include <utility>
#include <string>
#include <vector>
#include <functional>

namespace eclipse::gui {

    class MenuTab;

    class Component {
    public:
        /// @brief Initialize the component.
        virtual void onInit() = 0;

        /// @brief Update the component.
        virtual void onUpdate() = 0;

        /// @brief Get the component's ID. (unique identifier)
        [[nodiscard]] virtual const std::string& getId() const = 0;

        /// @brief Get the component's title. (used for sorting in the menu)
        [[nodiscard]] virtual const std::string& getTitle() const { return getId(); }
    };

    /// @brief Simple label component, that displays a title.
    class LabelComponent : public Component {
    public:
        explicit LabelComponent(std::string title) : m_title(std::move(title)) {}

        void onInit() override {}
        void onUpdate() override {}

        [[nodiscard]] const std::string& getId() const override { return m_title; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

    private:
        std::string m_title;
    };

    /// @brief Simple toggle component, that displays a title and a checkbox.
    class ToggleComponent : public Component {
    public:
        explicit ToggleComponent(std::string id, std::string title)
            : m_id(std::move(id)), m_title(std::move(title)) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ToggleComponent* callback(const std::function<void(bool)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Set toggle description.
        ToggleComponent* setDescription(std::string description) {
            m_description = std::move(description);
            return this;
        }

        /// @brief Add sub-component to toggle.
        void addOptions(std::function<void(MenuTab*)> options);
        
        /// @brief Get the toggle value.
        [[nodiscard]] bool getValue() const;

        /// @brief Set the toggle value.
        void setValue(bool value);

        /// @brief Allows to set keybinds for the toggle.
        ToggleComponent* handleKeybinds();

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] const std::string& getDescription() const { return m_description; }
        [[nodiscard]] MenuTab* getOptions() const { return m_options; }

        void triggerCallback(bool value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        std::string m_description;
        std::function<void(bool)> m_callback;
        MenuTab* m_options = nullptr;
    };

    /// @brief Radio button component for selecting one of the options.
    class RadioButtonComponent : public Component {
    public:
        explicit RadioButtonComponent(std::string id, std::string title, int value)
            : m_id(std::move(id)), m_title(std::move(title)), m_value(value) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        RadioButtonComponent* callback(const std::function<void(int)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Get the radio button value.
        [[nodiscard]] int getValue() const { return m_value; }

        /// @brief Set the radio button value.
        void setValue(int value) { m_value = value; }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        void triggerCallback(int value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        int m_value;
        std::function<void(int)> m_callback;
    };

    /// @brief Combo component for selecting one of the options.
    class ComboComponent : public Component {
    public:
        explicit ComboComponent(std::string id, std::string title, std::vector<std::string> items, int value)
            : m_id(std::move(id)), m_title(std::move(title)), m_items(items), m_value(value) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ComboComponent* callback(const std::function<void(int)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Get the combo value.
        [[nodiscard]] int getValue() const { return m_value; }

        /// @brief Get the combo items.
        [[nodiscard]] const std::vector<std::string>& getItems() const { return m_items; }

        /// @brief Set the combo button value.
        void setValue(int value) { m_value = value; }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        void triggerCallback(int value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        int m_value;
        std::vector<std::string> m_items;
        std::function<void(int)> m_callback;
    };

    /// @brief Slider component to select a value from a range.
    class SliderComponent : public Component {
    public:
        explicit SliderComponent(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f")
            : m_title(std::move(title)), m_id(std::move(id)), m_min(min), m_max(max), m_format(std::move(format)) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        SliderComponent* callback(const std::function<void(float)>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        [[nodiscard]] float getMin() const { return m_min; }
        [[nodiscard]] float getMax() const { return m_max; }
        [[nodiscard]] const std::string& getFormat() const { return m_format; }

        void triggerCallback(float value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        float m_min;
        float m_max;
        std::function<void(float)> m_callback;
    };
    
    /// @brief Input float component to input a value from a range.
    class InputFloatComponent : public Component {
    public:
        explicit InputFloatComponent(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f")
            : m_title(std::move(title)), m_id(std::move(id)), m_min(min), m_max(max), m_format(std::move(format)) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputFloatComponent* callback(const std::function<void(float)>& func) {
            m_callback = func;
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        [[nodiscard]] float getMin() const { return m_min; }
        [[nodiscard]] float getMax() const { return m_max; }
        [[nodiscard]] const std::string& getFormat() const { return m_format; }

        void triggerCallback(float value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        float m_min;
        float m_max;
        std::function<void(float)> m_callback;
    };

    /// @brief Input int component to input a value from a range.
    class InputIntComponent : public Component {
    public:
        explicit InputIntComponent(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX)
            : m_title(std::move(title)), m_id(std::move(id)), m_min(min), m_max(max) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputIntComponent* callback(const std::function<void(int)>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        [[nodiscard]] int getMin() const { return m_min; }
        [[nodiscard]] int getMax() const { return m_max; }

        void triggerCallback(int value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        int m_min;
        int m_max;
        std::function<void(int)> m_callback;
    };

    /// @brief Input float component to input a value from a range. Can be toggled .
    class FloatToggleComponent : public Component {
    public:
        explicit FloatToggleComponent(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f")
            : m_title(std::move(title)), m_id(std::move(id)), m_min(min), m_max(max), m_format(std::move(format)) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        FloatToggleComponent* toggleCallback(const std::function<void()>& func) { 
            m_toggleCallback = func; 
            return this;
        }
        FloatToggleComponent* valueCallback(const std::function<void(float)>& func) { 
            m_valueCallback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        [[nodiscard]] float getMin() const { return m_min; }
        [[nodiscard]] float getMax() const { return m_max; }
        [[nodiscard]] const std::string& getFormat() const { return m_format; }

        void triggerCallback(float value) {
            if (m_valueCallback) m_valueCallback(value);
        }
        void triggerCallback() {
            if (m_toggleCallback) m_toggleCallback();
        }

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        float m_min;
        float m_max;
        std::function<void(float)> m_valueCallback;
        std::function<void()> m_toggleCallback;
    };

    /// @brief Input text component to get user input as a string.
    class InputTextComponent : public Component {
    public:
        explicit InputTextComponent(std::string title, std::string id)
            : m_title(std::move(title)), m_id(std::move(id)) {}

        void onInit() override {};
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputTextComponent* callback(const std::function<void(std::string)>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        void triggerCallback(std::string value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        std::function<void(std::string)> m_callback;
    };

    /// @brief Input text component to get user input as a string.
    class ColorComponent : public Component {
    public:
        explicit ColorComponent(std::string title, std::string id)
            : m_title(std::move(title)), m_id(std::move(id)) {}

        void onInit() override {};
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ColorComponent* callback(const std::function<void(gui::Color)>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        void triggerCallback(gui::Color value) {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        std::function<void(gui::Color)> m_callback;
    };

    /// @brief Button component to execute an action when pressed.
    class ButtonComponent : public Component {
    public:
        explicit ButtonComponent(std::string title)
            : m_title(std::move(title)) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ButtonComponent* callback(const std::function<void()>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_title; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        void triggerCallback() {
            if (m_callback) m_callback();
        }

    private:
        std::string m_title;
        std::function<void()> m_callback;
    };

    /// @brief Component for picking a keybind.
    class KeybindComponent : public Component {
    public:
        explicit KeybindComponent(std::string title, std::string id, bool canDelete = false) :
            m_title(std::move(title)), m_id(std::move(id)), m_canDelete(canDelete) {}

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        KeybindComponent* callback(const std::function<void(keybinds::Keys)>& func) {
            m_callback = func;
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] bool canDelete() const { return m_canDelete; }

        void triggerCallback(keybinds::Keys key) {
            if (m_callback) m_callback(key);
        }

    private:
        std::string m_id;
        std::string m_title;
        bool m_canDelete;
        std::function<void(keybinds::Keys)> m_callback;
    };

    /// @brief Contains a list of components and a title, to be passed into render engine.
    class MenuTab {
    public:
        explicit MenuTab(std::string title) : m_title(std::move(title)) {}

        /// @brief Add a component to the tab.
        void addComponent(Component* component);

        /// @brief Remove a component from the tab.
        void removeComponent(Component* component);

        /// @brief Add a label to the tab.
        LabelComponent* addLabel(const std::string& title) {
            auto* label = new LabelComponent(title);
            addComponent(label);
            return label;
        }

        /// @brief Add a checkbox to the tab.
        ToggleComponent* addToggle(const std::string& title, const std::string& id) {
            auto* toggle = new ToggleComponent(id, title);
            addComponent(toggle);
            return toggle;
        }

        /// @brief Add a radio button to the tab.
        RadioButtonComponent* addRadioButton(const std::string& title, const std::string& id, int value) {
            auto* button = new RadioButtonComponent(id, title, value);
            addComponent(button);
            return button;
        }

        /// @brief Add a radio button to the tab.
        ComboComponent* addCombo(const std::string& title, const std::string& id, std::vector<std::string> items, int value) {
            auto* combo = new ComboComponent(id, title, items, value);
            addComponent(combo);
            return combo;
        }

        /// @brief Add a slider to the tab.
        SliderComponent* addSlider(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f") {
            auto* slider = new SliderComponent(title, id, min, max, format);
            addComponent(slider);
            return slider;
        }

        /// @brief Add an input float to the tab.
        InputFloatComponent* addInputFloat(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f") {
            auto* inputfloat = new InputFloatComponent(title, id, min, max, format);
            addComponent(inputfloat);
            return inputfloat;
        }

        /// @brief Add an input int to the tab.
        InputIntComponent* addInputInt(const std::string& title, const std::string& id, int min = INT_MIN, int max = INT_MAX) {
            auto* inputint = new InputIntComponent(title, id, min, max);
            addComponent(inputint);
            return inputint;
        }

        /// @brief Add an float toggle to the tab.
        FloatToggleComponent* addFloatToggle(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f") {
            auto* floattoggle = new FloatToggleComponent(title, id, min, max, format);
            addComponent(floattoggle);
            return floattoggle;
        }

        /// @brief Add an input text to the tab.
        InputTextComponent* addInputText(const std::string& title, const std::string& id) {
            auto* inputtext = new InputTextComponent(title, id);
            addComponent(inputtext);
            return inputtext;
        }

        /// @brief Add a button to the tab.
        ButtonComponent* addButton(const std::string& title) {
            auto* button = new ButtonComponent(title);
            addComponent(button);
            return button;
        }

        /// @brief Add a color picker to the tab.
        ColorComponent* addColorComponent(const std::string& title, const std::string& id) {
            auto* color = new ColorComponent(title, id);
            addComponent(color);
            return color;
        }

        /// @brief Add a keybind to the tab.
        KeybindComponent* addKeybind(const std::string& title, const std::string& id, bool canDelete = false) {
            auto* keybind = new KeybindComponent(title, id, canDelete);
            addComponent(keybind);
            return keybind;
        }

        /// @brief Get the tab's title.
        [[nodiscard]] const std::string& getTitle() const { return m_title; }

        /// @brief Get the tab's components.
        [[nodiscard]] const std::vector<Component*>& getComponents() const { return m_components; }

        /// @brief Find a tab by name (or create a new one if it does not exist).
        static MenuTab* find(const std::string& name);

    private:
        std::string m_title;
        std::vector<Component*> m_components;
    };

    class Layout {
    public:
        explicit Layout() : m_isToggled(false) {}

        /// @brief Draw the UI.
        virtual void draw() {}

        /// @brief Handle the component.
        virtual void visit(Component* component) {}

        /// @brief Toggle the menu.
        virtual void toggle() {
            m_isToggled = !m_isToggled;
        }

        /// @brief Get if the menu is toggled.
        [[nodiscard]] bool isToggled() { return m_isToggled; }

    protected:
        bool m_isToggled;
        // this'll also handle the window positions and all the imgui components
    };

    class Theme {
    public:
        explicit Theme(Layout* lay) : m_layout(lay) {}

        /// @brief Save the theme to a file.
        void saveToFile(const std::filesystem::path& path) {}

        /// @brief Set up the UI.
        virtual void setup() {}

        /// @brief Get the layout of the theme.
        [[nodiscard]] Layout* getLayout() { return m_layout; }

    private:
        Layout* m_layout;
        // plus like colors and settings that the user can change (also theme saving to file)
    };

    /// @brief Abstract class, that wraps all UI function calls.
    class Engine {
    public:
        /// @brief Get the UI engine instance. (ImGui for desktop, Cocos2d for mobile)
        static Engine* get();

        /// @brief Initialize the UI engine.
        virtual void init() = 0;

        /// @brief Toggle the UI visibility.
        virtual void toggle() = 0;

        /// @brief Toggle the UI visibility.
        virtual bool isToggled() = 0;

        /// @brief Find a tab by name.
        virtual MenuTab* findTab(const std::string& name) = 0;
    };


}