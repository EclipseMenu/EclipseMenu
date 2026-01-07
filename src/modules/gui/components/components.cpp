#include "button.hpp"
#include "color.hpp"
#include "combo.hpp"
#include "filesystem-combo.hpp"
#include "float-toggle.hpp"
#include "input-float.hpp"
#include "input-int.hpp"
#include "input-text.hpp"
#include "int-toggle.hpp"
#include "keybind.hpp"
#include "label-settings.hpp"
#include "label.hpp"
#include "radio.hpp"
#include "slider.hpp"
#include "toggle.hpp"

#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/labels/setting.hpp>

namespace eclipse::gui {
    template <typename T>
    T get_value(std::string_view key, T defaultValue, bool useTemp) {
        return useTemp
                   ? config::getTemp<T>(key, defaultValue)
                   : config::get<T>(key, defaultValue);
    }

    template <typename T>
    void store_value(std::string_view key, T value, bool useTemp) {
        useTemp
            ? config::setTemp<T>(key, std::move(value))
            : config::set<T>(key, std::move(value));
    }

    template <typename T>
    void store_value_ref(std::string_view key, T const& value, bool useTemp) {
        useTemp
            ? config::setTemp<T>(key, value)
            : config::set<T>(key, value);
    }

    #pragma region BaseComponent

    size_t Component::m_uniqueID = 0;

    bool ToggleComponent::getValue() const {
        return get_value(m_id, false, m_noSave);
    }

    void ToggleComponent::setValue(bool value) {
        store_value(m_id, value, m_noSave);
    }

    bool operator&(ComponentFlags lhs, ComponentFlags rhs) {
        return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
    }
    ComponentFlags operator|(ComponentFlags lhs, ComponentFlags rhs) {
        return static_cast<ComponentFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    }

    Component::Component() { m_uid = m_uniqueID++; }

    Component* Component::find(size_t uid) {
        for (auto& tab : Engine::get().getTabs()) {
            for (auto& component : tab.getComponents()) {
                if (component->getUID() == uid) {
                    return component.get();
                }
            }
        }

        return nullptr;
    }

    std::string const& Component::getTitle() const { return getId(); }
    std::string const& Component::getDescription() const { return m_description; }
    ComponentType Component::getType() const { return m_type; }
    Component* Component::disableSaving() { m_noSave = true; return this; }
    bool Component::isSaveDisabled() const { return m_noSave; }

    Component* Component::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    ComponentFlags Component::getFlags() const { return m_flags; }
    Component* Component::addFlag(ComponentFlags flag) {
        m_flags = m_flags | flag;
        return this;
    }
    Component* Component::removeFlag(ComponentFlags flag) {
        if (m_flags & flag) {
            m_flags = static_cast<ComponentFlags>(static_cast<uint8_t>(m_flags) & ~static_cast<uint8_t>(flag));
        }
        return this;
    }
    Component* Component::setFlags(ComponentFlags flags) { m_flags = flags; return this; }

    #pragma endregion BaseComponent

    #pragma region ButtonComponent

    ButtonComponent::ButtonComponent(std::string title): m_title(std::move(title)) {
        m_type = ComponentType::Button;
    }

    ButtonComponent* ButtonComponent::callback(Function<void()>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::string const& ButtonComponent::getId() const { return m_title; }
    std::string const& ButtonComponent::getTitle() const { return m_title; }

    ButtonComponent* ButtonComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    ButtonComponent* ButtonComponent::setDescription() {
        m_description = fmt::format("{}.desc", m_title);
        return this;
    }

    ButtonComponent* ButtonComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(
            fmt::format("button.{}", m_title), m_title, [this](bool down) {
                if (!down) return;
                this->triggerCallback();
            }
        );
        m_hasKeybind = true;
        return this;
    }

    bool ButtonComponent::hasKeybind() const { return m_hasKeybind; }

    void ButtonComponent::triggerCallback() {
        if (m_callback) m_callback();
    }

    #pragma endregion ButtonComponent

    #pragma region ColorComponent

    ColorComponent::ColorComponent(std::string title, std::string id, bool hasOpacity): m_id(std::move(id)),
        m_title(std::move(title)), m_hasOpacity(hasOpacity) {
        m_type = ComponentType::Color;
    }

    ColorComponent* ColorComponent::callback(Function<void(Color)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::string const& ColorComponent::getId() const { return m_id; }
    std::string const& ColorComponent::getTitle() const { return m_title; }
    bool ColorComponent::hasOpacity() const { return m_hasOpacity; }

    Color ColorComponent::getValue() const {
        return get_value(m_id, Colors::BLACK, m_noSave);
    }

    void ColorComponent::setValue(Color const& value) const {
        store_value_ref(m_id, value, m_noSave);
    }

    ColorComponent* ColorComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void ColorComponent::triggerCallback(Color const& value) {
        if (m_callback) m_callback(value);
    }

    #pragma endregion ColorComponent

    #pragma region ComboComponent

    ComboComponent::ComboComponent(
        std::string id, std::string title, std::vector<std::string> items, int value
    ): m_id(std::move(id)), m_title(std::move(title)), m_value(value), m_items(std::move(items)) {
        m_type = ComponentType::Combo;
        setValueIfEmpty(m_value);
    }

    ComboComponent* ComboComponent::callback(Function<void(int)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    int ComboComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    std::vector<std::string> const& ComboComponent::getItems() const { return m_items; }
    void ComboComponent::setItems(std::vector<std::string> const& items) { m_items = items; }

    void ComboComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    void ComboComponent::setValueIfEmpty(int value) const {
        if (!config::has(m_id)) store_value(m_id, value, m_noSave);
    }

    std::string const& ComboComponent::getId() const { return m_id; }
    std::string const& ComboComponent::getTitle() const { return m_title; }

    ComboComponent* ComboComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    ComboComponent* ComboComponent::setDescription() {
        m_description = fmt::format("{}.desc", m_title);
        return this;
    }

    void ComboComponent::triggerCallback(int value) {
        if (m_callback) m_callback(value);
    }

    #pragma endregion ComboComponent

    #pragma region FilesystemComboComponent

    FilesystemComboComponent::FilesystemComboComponent(
        std::string id, std::string title,
        std::filesystem::path directory
    ): m_id(std::move(id)), m_title(std::move(title)), m_directory(std::move(directory)) {
        m_type = ComponentType::FilesystemCombo;

        globFiles();
        if (getValue().empty() && m_items.size() > 0) setValue(0);
    }

    void FilesystemComboComponent::onUpdate() {
        globFiles();
    }

    FilesystemComboComponent* FilesystemComboComponent::callback(Function<void(int)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::filesystem::path FilesystemComboComponent::getValue() const {
        return get_value(m_id, std::filesystem::path(), m_noSave);
    }

    std::vector<std::filesystem::path> const& FilesystemComboComponent::getItems() const { return m_items; }

    void FilesystemComboComponent::setValue(std::filesystem::path path) const {
        store_value(m_id, std::move(path), m_noSave);
    }

    void FilesystemComboComponent::setValue(int index) const {
        setValue(m_items[index]);
    }

    std::string const& FilesystemComboComponent::getId() const { return m_id; }
    std::string const& FilesystemComboComponent::getTitle() const { return m_title; }
    std::string* FilesystemComboComponent::getSearchBuffer() { return &m_searchBuffer; }

    FilesystemComboComponent* FilesystemComboComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void FilesystemComboComponent::triggerCallback(int value) {
        if (m_callback) m_callback(value);
    }

    void FilesystemComboComponent::globFiles() {
        m_items.clear();

        std::error_code ec;
        if (!std::filesystem::exists(m_directory, ec)) return;

        for (auto const& entry : std::filesystem::recursive_directory_iterator(m_directory, ec)) {
            m_items.push_back(entry.path());
        }
    }

    #pragma endregion FilesystemComboComponent

    #pragma region FloatToggleComponent

    FloatToggleComponent::FloatToggleComponent(
        std::string title, std::string id, float min, float max,
        std::string format
    ): m_id(std::move(id)), m_title(std::move(title)), m_format(std::move(format)), m_min(min), m_max(max) {
        m_type = ComponentType::FloatToggle;
    }

    FloatToggleComponent* FloatToggleComponent::toggleCallback(Function<void()>&& func) {
        m_toggleCallback = std::move(func);
        return this;
    }

    FloatToggleComponent* FloatToggleComponent::valueCallback(Function<void(float)>&& func) {
        m_valueCallback = std::move(func);
        return this;
    }

    FloatToggleComponent* FloatToggleComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    FloatToggleComponent* FloatToggleComponent::setDescription() {
        m_description = fmt::format("{}.desc", m_title);
        return this;
    }

    FloatToggleComponent* FloatToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(
            m_id, m_title, [this](bool down) {
                if (!down) return;
                bool value = !config::get<bool>(fmt::format("{}.toggle", this->getId()), false);
                auto id = fmt::format("{}.toggle", this->getId());
                m_noSave
                    ? config::setTemp(id, value)
                    : config::set(id, value);
                this->triggerCallback();
            }
        );
        m_hasKeybind = true;
        return this;
    }

    std::string const& FloatToggleComponent::getId() const { return m_id; }
    std::string const& FloatToggleComponent::getTitle() const { return m_title; }
    bool FloatToggleComponent::hasKeybind() const { return m_hasKeybind; }
    float FloatToggleComponent::getMin() const { return m_min; }
    float FloatToggleComponent::getMax() const { return m_max; }
    std::string const& FloatToggleComponent::getFormat() const { return m_format; }

    float FloatToggleComponent::getValue() const {
        return get_value(m_id, 0.f, m_noSave);
    }

    void FloatToggleComponent::setValue(float value) const {
        store_value(m_id, value, m_noSave);
    }

    bool FloatToggleComponent::getState() const {
        return get_value(fmt::format("{}.toggle", this->getId()), false, m_noSave);
    }

    void FloatToggleComponent::setState(bool value) const {
        store_value(fmt::format("{}.toggle", this->getId()), value, m_noSave);
    }

    void FloatToggleComponent::triggerCallback(float value) {
        if (m_valueCallback) m_valueCallback(value);
    }

    void FloatToggleComponent::triggerCallback() {
        if (m_toggleCallback) m_toggleCallback();
    }

    #pragma endregion FloatToggleComponent

    #pragma region InputFloatComponent

    InputFloatComponent::InputFloatComponent(
        std::string title, std::string id, float min, float max,
        std::string format
    ): m_id(std::move(id)), m_title(std::move(title)), m_format(std::move(format)), m_min(min), m_max(max) {
        m_type = ComponentType::InputFloat;
    }

    InputFloatComponent* InputFloatComponent::callback(Function<void(float)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::string const& InputFloatComponent::getId() const { return m_id; }
    std::string const& InputFloatComponent::getTitle() const { return m_title; }
    float InputFloatComponent::getMin() const { return m_min; }
    float InputFloatComponent::getMax() const { return m_max; }
    std::string const& InputFloatComponent::getFormat() const { return m_format; }
    void InputFloatComponent::setMin(float min) { m_min = min; }
    void InputFloatComponent::setMax(float max) { m_max = max; }
    void InputFloatComponent::setFormat(std::string format) { m_format = std::move(format); }

    float InputFloatComponent::getValue() const {
        return get_value(m_id, 0.f, m_noSave);
    }

    void InputFloatComponent::setValue(float value) const {
        store_value(m_id, value, m_noSave);
    }

    InputFloatComponent* InputFloatComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    InputFloatComponent* InputFloatComponent::setDescription() {
        m_description = fmt::format("{}.desc", m_title);
        return this;
    }

    void InputFloatComponent::triggerCallback(float value) {
        if (m_callback) m_callback(value);
    }

    #pragma endregion InputFloatComponent

    #pragma region InputIntComponent

    InputIntComponent::InputIntComponent(std::string title, std::string id, int min, int max): m_id(std::move(id)),
        m_title(std::move(title)), m_min(min), m_max(max) {
        m_type = ComponentType::InputInt;
    }

    InputIntComponent* InputIntComponent::callback(Function<void(int)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::string const& InputIntComponent::getId() const { return m_id; }
    std::string const& InputIntComponent::getTitle() const { return m_title; }
    int InputIntComponent::getMin() const { return m_min; }
    int InputIntComponent::getMax() const { return m_max; }

    int InputIntComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    void InputIntComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    InputIntComponent* InputIntComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void InputIntComponent::triggerCallback(int value) {
        if (m_callback) m_callback(value);
    }

    #pragma endregion InputIntComponent

    #pragma region InputTextComponent

    InputTextComponent::InputTextComponent(std::string title, std::string id): m_id(std::move(id)),
                                                                               m_title(std::move(title)) {
        m_type = ComponentType::InputText;
    }

    InputTextComponent* InputTextComponent::callback(Function<void(std::string)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::string const& InputTextComponent::getId() const { return m_id; }
    std::string const& InputTextComponent::getTitle() const { return m_title; }

    std::string InputTextComponent::getValue() const {
        return get_value<std::string>(m_id, "", m_noSave);
    }

    void InputTextComponent::setValue(std::string value) const {
        store_value_ref(m_id, std::move(value), m_noSave);
    }

    InputTextComponent* InputTextComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void InputTextComponent::triggerCallback(std::string value) {
        if (m_callback) m_callback(std::move(value));
    }

    #pragma endregion InputTextComponent

    #pragma region IntToggleComponent

    IntToggleComponent::IntToggleComponent(std::string title, std::string id, int min, int max): m_id(std::move(id)),
        m_title(std::move(title)), m_min(min), m_max(max) {
        m_type = ComponentType::IntToggle;
    }

    IntToggleComponent* IntToggleComponent::toggleCallback(Function<void()>&& func) {
        m_toggleCallback = std::move(func);
        return this;
    }

    IntToggleComponent* IntToggleComponent::valueCallback(Function<void(int)>&& func) {
        m_valueCallback = std::move(func);
        return this;
    }

    IntToggleComponent* IntToggleComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    IntToggleComponent* IntToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(
            m_id, m_title, [this](bool down) {
                if (!down) return;
                bool value = !config::get<bool>(fmt::format("{}.toggle", this->getId()), false);
                auto id = fmt::format("{}.toggle", this->getId());
                m_noSave
                    ? config::setTemp(id, value)
                    : config::set(id, value);
                this->triggerCallback();
            }
        );
        m_hasKeybind = true;
        return this;
    }

    std::string const& IntToggleComponent::getId() const { return m_id; }
    std::string const& IntToggleComponent::getTitle() const { return m_title; }
    bool IntToggleComponent::hasKeybind() const { return m_hasKeybind; }
    int IntToggleComponent::getMin() const { return m_min; }
    int IntToggleComponent::getMax() const { return m_max; }

    int IntToggleComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    void IntToggleComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    bool IntToggleComponent::getState() const {
        return get_value(fmt::format("{}.toggle", this->getId()), false, m_noSave);
    }

    void IntToggleComponent::setState(bool value) const {
        store_value(fmt::format("{}.toggle", this->getId()), value, m_noSave);
    }

    void IntToggleComponent::triggerCallback(int value) {
        if (m_valueCallback) m_valueCallback(value);
    }

    void IntToggleComponent::triggerCallback() {
        if (m_toggleCallback) m_toggleCallback();
    }

    #pragma endregion IntToggleComponent

    #pragma region KeybindComponent

    KeybindComponent::KeybindComponent(std::string title, std::string id, bool canDelete): m_id(std::move(id)),
        m_title(std::move(title)), m_canDelete(canDelete) {
        m_type = ComponentType::Keybind;
    }

    KeybindComponent* KeybindComponent::setInternal() {
        m_callback = [this](keybinds::Keys key) {
            auto keybind = keybinds::Manager::get()->getKeybind(m_id);
            if (!keybind.has_value()) return;
            auto& keybindRef = keybind->get();
            keybindRef.setKey(key);
        };
        return this;
    }

    KeybindComponent* KeybindComponent::setDefaultKey(keybinds::Keys key) {
        m_defaultKey = key;
        return this;
    }

    std::string const& KeybindComponent::getId() const { return m_id; }
    std::string const& KeybindComponent::getTitle() const { return m_title; }
    bool KeybindComponent::canDelete() const { return m_canDelete; }
    keybinds::Keys KeybindComponent::getDefaultKey() const { return m_defaultKey; }

    KeybindComponent* KeybindComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void KeybindComponent::triggerCallback(keybinds::Keys key) {
        if (m_callback) m_callback(key);
    }

    KeybindComponent* KeybindComponent::callback(Function<void(keybinds::Keys)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    #pragma endregion KeybindComponent

    #pragma region LabelComponent

    LabelComponent::LabelComponent(std::string title): m_title(std::move(title)) {
        m_type = ComponentType::Label;
    }

    void LabelComponent::setText(std::string text) { m_title = std::move(text); }
    std::string const& LabelComponent::getId() const { return m_title; }
    std::string const& LabelComponent::getTitle() const { return m_title; }

    LabelComponent* LabelComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    #pragma endregion LabelComponent

    #pragma region LabelSettingsComponent

    LabelSettingsComponent::LabelSettingsComponent(labels::LabelSettings* settings): m_settings(settings) {
        m_type = ComponentType::LabelSettings;
        m_id = fmt::format("label-{}", m_settings->id);
    }

    std::string const& LabelSettingsComponent::getId() const { return m_id; }
    std::string const& LabelSettingsComponent::getTitle() const { return m_settings->text; }
    labels::LabelSettings* LabelSettingsComponent::getSettings() const { return m_settings; }

    LabelSettingsComponent* LabelSettingsComponent::deleteCallback(Function<void()>&& func) {
        m_deleteCallback = std::move(func);
        return this;
    }

    LabelSettingsComponent* LabelSettingsComponent::editCallback(Function<void()>&& func) {
        m_editCallback = std::move(func);
        return this;
    }

    LabelSettingsComponent* LabelSettingsComponent::moveCallback(Function<void(bool)>&& func) {
        m_moveCallback = std::move(func);
        return this;
    }

    LabelSettingsComponent* LabelSettingsComponent::exportCallback(Function<void()>&& func) {
        m_exportCallback = std::move(func);
        return this;
    }

    void LabelSettingsComponent::triggerDeleteCallback() {
        if (m_deleteCallback) m_deleteCallback();

        // We also have to clean up the keybind
        keybinds::Manager::get()->unregisterKeybind(fmt::format("label.{}", m_settings->id));
    }

    void LabelSettingsComponent::triggerEditCallback() {
        if (m_editCallback) m_editCallback();

        // Update the keybind title
        auto keybind = keybinds::Manager::get()->getKeybind(fmt::format("label.{}", m_settings->id));
        if (keybind.has_value()) {
            keybind->get().setTitle(m_settings->name);
        } else {
            geode::log::warn("Keybind with ID 'label.{}' not found", m_settings->id);
        }
    }

    void LabelSettingsComponent::triggerMoveCallback(bool up) {
        if (m_moveCallback) m_moveCallback(up);
    }

    void LabelSettingsComponent::triggerExportCallback() {
        if (m_exportCallback) m_exportCallback();
    }

    LabelSettingsComponent* LabelSettingsComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(
            fmt::format("label.{}", m_settings->id), m_settings->name, [this](bool down) {
                if (!down) return;
                this->m_settings->visible = !this->m_settings->visible;
                this->triggerEditCallback();
            }
        );
        m_hasKeybind = true;
        return this;
    }

    bool LabelSettingsComponent::hasKeybind() const { return m_hasKeybind; }

    #pragma endregion LabelSettingsComponent

    #pragma region RadioButtonComponent

    RadioButtonComponent::RadioButtonComponent(std::string id, std::string title, int value): m_id(std::move(id)),
        m_title(std::move(title)), m_value(value) {
        m_type = ComponentType::RadioButton;
    }

    RadioButtonComponent* RadioButtonComponent::callback(Function<void(int)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    int RadioButtonComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    void RadioButtonComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    int RadioButtonComponent::getChoice() const { return m_value; }

    RadioButtonComponent* RadioButtonComponent::handleKeybinds() {
        auto specialId = fmt::format("{}-{}", m_id, m_value);
        keybinds::Manager::get()->registerKeybind(
            specialId, m_title, [this](bool down) {
                if (!down) return;
                auto value = getChoice();
                setValue(value);
                this->triggerCallback(value);
            }
        );
        m_hasKeybind = true;
        return this;
    }

    std::string const& RadioButtonComponent::getId() const { return m_id; }
    std::string const& RadioButtonComponent::getTitle() const { return m_title; }
    bool RadioButtonComponent::hasKeybind() const { return m_hasKeybind; }

    void RadioButtonComponent::triggerCallback(int value) {
        if (m_callback) m_callback(value);
    }

    RadioButtonComponent* RadioButtonComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    #pragma endregion RadioButtonComponent

    #pragma region SliderComponent

    SliderComponent::SliderComponent(
        std::string title, std::string id, float min, float max, std::string format
    ): m_id(std::move(id)), m_title(std::move(title)), m_format(std::move(format)), m_min(min), m_max(max) {
        m_type = ComponentType::Slider;
    }

    SliderComponent* SliderComponent::callback(Function<void(float)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    std::string const& SliderComponent::getId() const { return m_id; }
    std::string const& SliderComponent::getTitle() const { return m_title; }
    float SliderComponent::getMin() const { return m_min; }
    float SliderComponent::getMax() const { return m_max; }
    std::string const& SliderComponent::getFormat() const { return m_format; }

    float SliderComponent::getValue() const {
        return get_value(m_id, 0.f, m_noSave);
    }

    void SliderComponent::setValue(float value) const {
        store_value(m_id, value, m_noSave);
    }

    SliderComponent* SliderComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void SliderComponent::triggerCallback(float value) {
        if (m_callback) m_callback(value);
    }

    #pragma endregion SliderComponent

    #pragma region ToggleComponent

    ToggleComponent::ToggleComponent(std::string id, std::string title)
        : m_id(std::move(id)), m_title(std::move(title)) {
        m_type = ComponentType::Toggle;
    }

    ToggleComponent* ToggleComponent::callback(Function<void(bool)>&& func) {
        m_callback = std::move(func);
        return this;
    }

    void ToggleComponent::addOptions(FunctionRef<void(MenuTab*)> options) {
        m_options = std::make_unique<MenuTab>(m_title, false);
        options(m_options.get());
    }

    ToggleComponent* ToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(
            m_id, m_title, [this](bool down) {
                if (!down) return;
                bool value = !getValue();
                setValue(value);
                this->triggerCallback(value);
            }
        );
        m_hasKeybind = true;
        return this;
    }

    ToggleComponent* ToggleComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    ToggleComponent* ToggleComponent::setDescription() {
        m_description = fmt::format("{}.desc", m_title);
        return this;
    }

    std::string const& ToggleComponent::getId() const { return m_id; }
    std::string const& ToggleComponent::getTitle() const { return m_title; }
    MenuTab* ToggleComponent::getOptions() const { return m_options.get(); }
    bool ToggleComponent::hasKeybind() const { return m_hasKeybind; }

    void ToggleComponent::triggerCallback(bool value) {
        if (m_callback) m_callback(value);
    }

    #pragma endregion ToggleComponent
}
