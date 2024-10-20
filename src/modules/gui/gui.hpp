#pragma once

#include <modules/keybinds/manager.hpp>
#include <modules/labels/setting.hpp>
#include <modules/gui/color.hpp>

#include <utility>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include "color.hpp"
#include "popup.hpp"

namespace eclipse::gui {

    class MenuTab;

    enum class ComponentType {
        Unknown = -1,
        Label, Toggle, RadioButton,
        Combo, Slider, InputFloat, InputInt,
        FloatToggle, InputText, Color,
        Button, Keybind, LabelSettings, FilesystemCombo, IntToggle
    };

    class Component {
    public:
        virtual ~Component() = default;

        /// @brief Initialize the component.
        virtual void onInit() = 0;

        /// @brief Update the component.
        virtual void onUpdate() = 0;

        /// @brief Get the component's ID. (unique identifier)
        [[nodiscard]] virtual const std::string& getId() const = 0;

        /// @brief Get the component's title. (used for sorting in the menu)
        [[nodiscard]] virtual const std::string& getTitle() const { return getId(); }

        /// @brief Get the component's description.
        [[nodiscard]] const std::string& getDescription() const { return m_description; }

        /// @brief Get the component's type (used to skip dynamic casting)
        [[nodiscard]] ComponentType getType() const { return m_type; }

        /// @brief Excludes value from being saved into main configuration file
        /// Useful for some internal values. (Uses temporary storage).
        void disableSaving() { m_noSave = true; }

        /// @brief Whether current component should use temporary storage
        [[nodiscard]] bool isSaveDisabled() const { return m_noSave; }

        /// @brief Set the component's description.
        virtual Component* setDescription(std::string description) {
            m_description = std::move(description);
            return this;
        }

    protected:
        ComponentType m_type = ComponentType::Unknown;
        bool m_noSave = false;
        std::string m_description;
    };

    /// @brief Simple label component, that displays a title.
    class LabelComponent : public Component {
    public:
        explicit LabelComponent(std::string title) : m_title(std::move(title)) {
            m_type = ComponentType::Label;
        }

        void onInit() override {}
        void onUpdate() override {}

        void setText(std::string text) { m_title = std::move(text); }

        [[nodiscard]] const std::string& getId() const override { return m_title; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        LabelComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

    private:
        std::string m_title;
    };

    /// @brief Simple toggle component, that displays a title and a checkbox.
    class ToggleComponent : public Component {
    public:
        explicit ToggleComponent(std::string id, std::string title)
            : m_id(std::move(id)), m_title(std::move(title)) {
            m_type = ComponentType::Toggle;
        }

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ToggleComponent* callback(const std::function<void(bool)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Add sub-component to toggle.
        void addOptions(const std::function<void(std::shared_ptr<MenuTab>)>& options);
        
        /// @brief Get the toggle value.
        [[nodiscard]] bool getValue() const;

        /// @brief Set the toggle value.
        void setValue(bool value) const;

        /// @brief Allows to set keybinds for the toggle.
        ToggleComponent* handleKeybinds();

        ToggleComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] std::weak_ptr<MenuTab> getOptions() const { return m_options; }
        [[nodiscard]] bool hasKeybind() const { return m_hasKeybind; }

        void triggerCallback(bool value) const {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        std::function<void(bool)> m_callback;
        std::shared_ptr<MenuTab> m_options = nullptr;
        bool m_hasKeybind = false;
    };

    /// @brief Radio button component for selecting one of the options.
    class RadioButtonComponent : public Component {
    public:
        explicit RadioButtonComponent(std::string id, std::string title, int value)
            : m_id(std::move(id)), m_title(std::move(title)), m_value(value) {
            m_type = ComponentType::RadioButton;
        }

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        RadioButtonComponent* callback(const std::function<void(int)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Get the radio button value.
        [[nodiscard]] int getValue() const;

        /// @brief Set the radio button value.
        void setValue(int value) const;

        [[nodiscard]] int getChoice() const { return m_value; }

        /// @brief Allows to set keybinds for the radio button.
        RadioButtonComponent* handleKeybinds();

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] bool hasKeybind() const { return m_hasKeybind; }

        void triggerCallback(int value) const {
            if (m_callback) m_callback(value);
        }

        RadioButtonComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

    private:
        std::string m_id;
        std::string m_title;
        int m_value;
        std::function<void(int)> m_callback;
        bool m_hasKeybind = false;
    };

    /// @brief Combo component for selecting one of the options.
    class ComboComponent : public Component {
    public:
        explicit ComboComponent(std::string id, std::string title, std::vector<std::string> items, int value)
            : m_id(std::move(id)), m_title(std::move(title)), m_value(value), m_items(std::move(items)) {
            m_type = ComponentType::Combo;
            setValueIfEmpty(m_value);
        }

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ComboComponent* callback(const std::function<void(int)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Get the combo value (selected item index).
        [[nodiscard]] int getValue() const;

        /// @brief Get the combo items.
        [[nodiscard]] const std::vector<std::string>& getItems() const { return m_items; }
        void setItems(const std::vector<std::string>& items) { m_items = items; }

        /// @brief Set the combo value (selected item index).
        void setValue(int value) const;

        /// @brief Set the combo value if empty (selected item index).
        void setValueIfEmpty(int value) const;

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        ComboComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(int value) const {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        int m_value;
        std::vector<std::string> m_items;
        std::function<void(int)> m_callback;
    };

    /// @brief Combo component for selecting entries from a path.
    class FilesystemComboComponent : public Component {
    public:
        explicit FilesystemComboComponent(std::string id, std::string title, std::filesystem::path directory)
            : m_id(std::move(id)), m_title(std::move(title)), m_directory(std::move(directory)) {
            m_type = ComponentType::FilesystemCombo;
        }

        void onInit() override {
            globFiles();

            if(getValue().empty() && m_items.size() > 0)
                setValue(0);
        }
        void onUpdate() override {
            globFiles();
        }

        /// @brief Set a callback function to be called when the component value changes.
        FilesystemComboComponent* callback(const std::function<void(int)>& func) { 
            m_callback = func; 
            return this;
        }

        /// @brief Get the combo value.
        [[nodiscard]] std::filesystem::path getValue() const;

        /// @brief Get the combo items.
        [[nodiscard]] const std::vector<std::filesystem::path>& getItems() const { return m_items; }

        /// @brief Set the combo value.
        void setValue(std::filesystem::path path) const;

        /// @brief Set the combo value.
        void setValue(int index);

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        [[nodiscard]] std::string* getSearchBuffer() { return &m_searchBuffer; }

        FilesystemComboComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(int value) const {
            if (m_callback) m_callback(value);
        }

    private:
        void globFiles();

    private:
        std::string m_id;
        std::string m_title;
        std::filesystem::path m_directory;
        std::vector<std::filesystem::path> m_items;
        std::function<void(int)> m_callback;

        std::string m_searchBuffer;
    };

    /// @brief Slider component to select a value from a range.
    class SliderComponent : public Component {
    public:
        explicit SliderComponent(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f")
            : m_id(std::move(id)), m_title(std::move(title)), m_format(std::move(format)), m_min(min), m_max(max) {
            m_type = ComponentType::Slider;
        }

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

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;

        SliderComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(float value) const {
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
            : m_id(std::move(id)), m_title(std::move(title)), m_format(std::move(format)), m_min(min), m_max(max) {
            m_type = ComponentType::InputFloat;
        }

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

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;

        InputFloatComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(float value) const {
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
            : m_id(std::move(id)), m_title(std::move(title)), m_min(min), m_max(max) {
            m_type = ComponentType::InputInt;
        }

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

        [[nodiscard]] int getValue() const;
        void setValue(int value) const;

        InputIntComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(int value) const {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        int m_min;
        int m_max;
        std::function<void(int)> m_callback;
    };

    /// @brief Input int component to input a value from a range. Can be toggled.
    class IntToggleComponent : public Component {
    public:
        explicit IntToggleComponent(std::string title, std::string id, int min = INT_MIN, int max = INT_MAX)
            : m_id(std::move(id)), m_title(std::move(title)), m_min(min), m_max(max) {
            m_type = ComponentType::IntToggle;
        }

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        IntToggleComponent* toggleCallback(const std::function<void()>& func) { 
            m_toggleCallback = func; 
            return this;
        }
        IntToggleComponent* valueCallback(const std::function<void(int)>& func) { 
            m_valueCallback = func; 
            return this;
        }

        /// @brief Set toggle description.
        IntToggleComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        /// @brief Allows to set keybinds for the toggle.
        IntToggleComponent* handleKeybinds();

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] bool hasKeybind() const { return m_hasKeybind; }

        [[nodiscard]] int getMin() const { return m_min; }
        [[nodiscard]] int getMax() const { return m_max; }

        [[nodiscard]] int getValue() const;
        void setValue(int value) const;
        [[nodiscard]] bool getState() const;
        void setState(bool value) const;

        void triggerCallback(int value) const {
            if (m_valueCallback) m_valueCallback(value);
        }
        void triggerCallback() const {
            if (m_toggleCallback) m_toggleCallback();
        }

    private:
        std::string m_id;
        std::string m_title;
        std::string m_description;
        int m_min;
        int m_max;
        std::function<void(int)> m_valueCallback;
        std::function<void()> m_toggleCallback;
        bool m_hasKeybind = false;
    };

    /// @brief Input float component to input a value from a range. Can be toggled .
    class FloatToggleComponent : public Component {
    public:
        explicit FloatToggleComponent(std::string title, std::string id, float min = FLT_MIN, float max = FLT_MAX, std::string format = "%.3f")
            : m_id(std::move(id)), m_title(std::move(title)), m_format(std::move(format)), m_min(min), m_max(max) {
            m_type = ComponentType::FloatToggle;
        }

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

        /// @brief Set toggle description.
        FloatToggleComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        /// @brief Allows to set keybinds for the toggle.
        FloatToggleComponent* handleKeybinds();

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] bool hasKeybind() const { return m_hasKeybind; }

        [[nodiscard]] float getMin() const { return m_min; }
        [[nodiscard]] float getMax() const { return m_max; }
        [[nodiscard]] const std::string& getFormat() const { return m_format; }

        [[nodiscard]] float getValue() const;
        void setValue(float value) const;
        [[nodiscard]] bool getState() const;
        void setState(bool value) const;

        void triggerCallback(float value) const {
            if (m_valueCallback) m_valueCallback(value);
        }
        void triggerCallback() const {
            if (m_toggleCallback) m_toggleCallback();
        }

    private:
        std::string m_id;
        std::string m_title;
        std::string m_format;
        std::string m_description;
        float m_min;
        float m_max;
        std::function<void(float)> m_valueCallback;
        std::function<void()> m_toggleCallback;
        bool m_hasKeybind = false;
    };

    /// @brief Input text component to get user input as a string.
    class InputTextComponent : public Component {
    public:
        explicit InputTextComponent(std::string title, std::string id)
            : m_id(std::move(id)), m_title(std::move(title)) {
            m_type = ComponentType::InputText;
        }

        void onInit() override {};
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        InputTextComponent* callback(const std::function<void(std::string)>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        [[nodiscard]] std::string getValue() const;
        void setValue(const std::string& value) const;

        InputTextComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(std::string value) const {
            if (m_callback) m_callback(std::move(value));
        }

    private:
        std::string m_id;
        std::string m_title;
        std::function<void(std::string)> m_callback;
    };

    /// @brief Input text component to get user input as a string.
    class ColorComponent : public Component {
    public:
        explicit ColorComponent(std::string title, std::string id, bool hasOpacity = false)
            : m_id(std::move(id)), m_title(std::move(title)), m_hasOpacity(hasOpacity) {
            m_type = ComponentType::Color;
        }

        void onInit() override {};
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ColorComponent* callback(const std::function<void(Color)>& func) {
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }
        [[nodiscard]] bool hasOpacity() const { return m_hasOpacity; }

        [[nodiscard]] Color getValue() const;
        void setValue(const Color& value) const;

        ColorComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(const Color& value) const {
            if (m_callback) m_callback(value);
        }

    private:
        std::string m_id;
        std::string m_title;
        bool m_hasOpacity;
        std::function<void(Color)> m_callback;
    };

    /// @brief Button component to execute an action when pressed.
    class ButtonComponent : public Component {
    public:
        explicit ButtonComponent(std::string title)
            : m_title(std::move(title)) {
            m_type = ComponentType::Button;
        }

        void onInit() override {}
        void onUpdate() override {}

        /// @brief Set a callback function to be called when the component value changes.
        ButtonComponent* callback(const std::function<void()>& func) { 
            m_callback = func; 
            return this;
        }

        [[nodiscard]] const std::string& getId() const override { return m_title; }
        [[nodiscard]] const std::string& getTitle() const override { return m_title; }

        ButtonComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        /// @brief Allows to set keybinds for the button.
        ButtonComponent* handleKeybinds();

        [[nodiscard]] bool hasKeybind() const { return m_hasKeybind; }

        void triggerCallback() const {
            if (m_callback) m_callback();
        }

    private:
        std::string m_title;
        std::function<void()> m_callback;
        bool m_hasKeybind = false;
    };

    /// @brief Component for picking a keybind.
    class KeybindComponent : public Component {
    public:
        explicit KeybindComponent(std::string title, std::string id, bool canDelete = false) :
            m_id(std::move(id)), m_title(std::move(title)), m_canDelete(canDelete) {
            m_type = ComponentType::Keybind;
        }

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

        KeybindComponent* setDescription(std::string description) override {
            m_description = std::move(description);
            return this;
        }

        void triggerCallback(keybinds::Keys key) const {
            if (m_callback) m_callback(key);
        }

    private:
        std::string m_id;
        std::string m_title;
        bool m_canDelete;
        std::function<void(keybinds::Keys)> m_callback;
    };

    /// @brief Component that allows to change label settings.
    class LabelSettingsComponent : public Component {
    public:
        explicit LabelSettingsComponent(labels::LabelSettings* settings)
            : m_settings(settings) {
            m_type = ComponentType::LabelSettings;
            m_id = fmt::format("label-{}", m_settings->id);
        }

        void onInit() override {}
        void onUpdate() override {}

        [[nodiscard]] const std::string& getId() const override { return m_id; }
        [[nodiscard]] const std::string& getTitle() const override { return m_settings->text; }

        labels::LabelSettings* getSettings() const { return m_settings; }

        LabelSettingsComponent* deleteCallback(const std::function<void()>& func) {
            m_deleteCallback = func;
            return this;
        }

        LabelSettingsComponent* editCallback(const std::function<void()>& func) {
            m_editCallback = func;
            return this;
        }

        LabelSettingsComponent* moveCallback(const std::function<void(bool)>& func) {
            m_moveCallback = func;
            return this;
        }

        void triggerDeleteCallback() const {
            if (m_deleteCallback) m_deleteCallback();
        }

        void triggerEditCallback() const {
            if (m_editCallback) m_editCallback();
        }

        void triggerMoveCallback(bool up) const {
            if (m_moveCallback) m_moveCallback(up);
        }

    private:
        std::string m_id;
        labels::LabelSettings* m_settings;
        std::function<void()> m_deleteCallback;
        std::function<void()> m_editCallback;
        std::function<void(bool)> m_moveCallback;
    };

    /// @brief Contains a list of components and a title, to be passed into render engine.
    class MenuTab {
    public:
        explicit MenuTab(std::string title) : m_title(std::move(title)) {}

        /// @brief Add a component to the tab.
        void addComponent(const std::shared_ptr<Component>& component);

        /// @brief Remove a component from the tab.
        void removeComponent(std::weak_ptr<Component> component);

        /// @brief Add a label to the tab.
        std::shared_ptr<LabelComponent> addLabel(const std::string& title) {
            auto label = std::make_shared<LabelComponent>(title);
            addComponent(label);
            return label;
        }

        /// @brief Add a checkbox to the tab.
        std::shared_ptr<ToggleComponent> addToggle(const std::string& title, const std::string& id) {
            auto toggle = std::make_shared<ToggleComponent>(id, title);
            addComponent(toggle);
            return toggle;
        }

        /// @brief Add a radio button to the tab.
        std::shared_ptr<RadioButtonComponent> addRadioButton(const std::string& title, const std::string& id, int value) {
            auto button = std::make_shared<RadioButtonComponent>(id, title, value);
            addComponent(button);
            return button;
        }

        /// @brief Add a combo to the tab.
        std::shared_ptr<ComboComponent> addCombo(const std::string& title, const std::string& id, std::vector<std::string> items, int value) {
            auto combo = std::make_shared<ComboComponent>(id, title, items, value);
            addComponent(combo);
            return combo;
        }

        /// @brief Add a filesystem combo button to the tab.
        std::shared_ptr<FilesystemComboComponent> addFilesystemCombo(const std::string& title, const std::string& id, std::filesystem::path directory) {
            auto combo = std::make_shared<FilesystemComboComponent>(id, title, directory);
            addComponent(combo);
            return combo;
        }

        /// @brief Add a slider to the tab.
        std::shared_ptr<SliderComponent> addSlider(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f") {
            auto slider = std::make_shared<SliderComponent>(title, id, min, max, format);
            addComponent(slider);
            return slider;
        }

        /// @brief Add an input float to the tab.
        std::shared_ptr<InputFloatComponent> addInputFloat(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f") {
            auto inputFloat = std::make_shared<InputFloatComponent>(title, id, min, max, format);
            addComponent(inputFloat);
            return inputFloat;
        }

        /// @brief Add an input int to the tab.
        std::shared_ptr<InputIntComponent> addInputInt(const std::string& title, const std::string& id, int min = INT_MIN, int max = INT_MAX) {
            auto inputInt = std::make_shared<InputIntComponent>(title, id, min, max);
            addComponent(inputInt);
            return inputInt;
        }

        /// @brief Add an float toggle to the tab.
        std::shared_ptr<IntToggleComponent> addIntToggle(const std::string& title, const std::string& id, int min = INT_MIN, int max = INT_MAX) {
            auto intToggle = std::make_shared<IntToggleComponent>(title, id, min, max);
            addComponent(intToggle);
            return intToggle;
        }

        /// @brief Add an float toggle to the tab.
        std::shared_ptr<FloatToggleComponent> addFloatToggle(const std::string& title, const std::string& id, float min = FLT_MIN, float max = FLT_MAX, const std::string& format = "%.3f") {
            auto floatToggle = std::make_shared<FloatToggleComponent>(title, id, min, max, format);
            addComponent(floatToggle);
            return floatToggle;
        }

        /// @brief Add an input text to the tab.
        std::shared_ptr<InputTextComponent> addInputText(const std::string& title, const std::string& id) {
            auto inputText = std::make_shared<InputTextComponent>(title, id);
            addComponent(inputText);
            return inputText;
        }

        /// @brief Add a button to the tab.
        std::shared_ptr<ButtonComponent> addButton(const std::string& title) {
            auto button = std::make_shared<ButtonComponent>(title);
            addComponent(button);
            return button;
        }

        /// @brief Add a color picker to the tab.
        std::shared_ptr<ColorComponent> addColorComponent(const std::string& title, const std::string& id, bool hasOpacity = false) {
            auto color = std::make_shared<ColorComponent>(title, id, hasOpacity);
            addComponent(color);
            return color;
        }

        /// @brief Add a keybind to the tab.
        std::shared_ptr<KeybindComponent> addKeybind(const std::string& title, const std::string& id, bool canDelete = false) {
            auto keybind = std::make_shared<KeybindComponent>(title, id, canDelete);
            addComponent(keybind);
            return keybind;
        }

        /// @brief Add a label settings to the tab.
        std::shared_ptr<LabelSettingsComponent> addLabelSetting(labels::LabelSettings* settings) {
            auto labelSettings = std::make_shared<LabelSettingsComponent>(settings);
            addComponent(labelSettings);
            return labelSettings;
        }

        /// @brief Get the tab's title.
        [[nodiscard]] const std::string& getTitle() const { return m_title; }

        /// @brief Get the tab's components.
        [[nodiscard]] const std::vector<std::shared_ptr<Component>>& getComponents() const { return m_components; }

        /// @brief Find a tab by name (or create a new one if it does not exist).
        static std::shared_ptr<MenuTab> find(std::string_view name);

    private:
        std::string m_title;
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
