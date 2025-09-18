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
            ? config::setTemp<T>(key, value)
            : config::set<T>(key, value);
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

    void ToggleComponent::setValue(bool value) const {
        store_value(m_id, value, m_noSave);
    }

    bool operator&(ComponentFlags lhs, ComponentFlags rhs) {
        return static_cast<uint8_t>(lhs) & static_cast<uint8_t>(rhs);
    }
    ComponentFlags operator|(ComponentFlags lhs, ComponentFlags rhs) {
        return static_cast<ComponentFlags>(static_cast<uint8_t>(lhs) | static_cast<uint8_t>(rhs));
    }

    Component::Component() { m_uid = m_uniqueID++; }

    std::shared_ptr<Component> Component::find(size_t uid) {
        const auto& engine = Engine::get();

        for (auto& tab : engine->getTabs()) {
            for (auto& component : tab->getComponents()) {
                if (component->getUID() == uid) {
                    return component;
                }
            }
        }

        return nullptr;
    }

    const std::string& Component::getTitle() const { return getId(); }
    const std::string& Component::getDescription() const { return m_description; }
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

    ButtonComponent* ButtonComponent::callback(const std::function<void()>& func) {
        m_callback = func;
        return this;
    }

    const std::string& ButtonComponent::getId() const { return m_title; }
    const std::string& ButtonComponent::getTitle() const { return m_title; }

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

    void ButtonComponent::triggerCallback() const {
        if (m_callback) m_callback();
    }

    #pragma endregion ButtonComponent

    #pragma region ColorComponent

    ColorComponent::ColorComponent(std::string title, std::string id, bool hasOpacity): m_id(std::move(id)),
        m_title(std::move(title)), m_hasOpacity(hasOpacity) {
        m_type = ComponentType::Color;
    }

    ColorComponent* ColorComponent::callback(const std::function<void(Color)>& func) {
        m_callback = func;
        return this;
    }

    const std::string& ColorComponent::getId() const { return m_id; }
    const std::string& ColorComponent::getTitle() const { return m_title; }
    bool ColorComponent::hasOpacity() const { return m_hasOpacity; }

    Color ColorComponent::getValue() const {
        return get_value(m_id, Color::BLACK, m_noSave);
    }

    void ColorComponent::setValue(const Color& value) const {
        store_value_ref(m_id, value, m_noSave);
    }

    ColorComponent* ColorComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void ColorComponent::triggerCallback(const Color& value) const {
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

    ComboComponent* ComboComponent::callback(const std::function<void(int)>& func) {
        m_callback = func;
        return this;
    }

    int ComboComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    const std::vector<std::string>& ComboComponent::getItems() const { return m_items; }
    void ComboComponent::setItems(const std::vector<std::string>& items) { m_items = items; }

    void ComboComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    void ComboComponent::setValueIfEmpty(int value) const {
        if (!config::has(m_id)) store_value(m_id, value, m_noSave);
    }

    const std::string& ComboComponent::getId() const { return m_id; }
    const std::string& ComboComponent::getTitle() const { return m_title; }

    ComboComponent* ComboComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    ComboComponent* ComboComponent::setDescription() {
        m_description = fmt::format("{}.desc", m_title);
        return this;
    }

    void ComboComponent::triggerCallback(int value) const {
        if (m_callback) m_callback(value);
    }

    #pragma endregion ComboComponent

    #pragma region FilesystemComboComponent

    FilesystemComboComponent::FilesystemComboComponent(
        std::string id, std::string title,
        std::filesystem::path directory
    ): m_id(std::move(id)), m_title(std::move(title)), m_directory(std::move(directory)) {
        m_type = ComponentType::FilesystemCombo;
    }

    void FilesystemComboComponent::onInit() {
        globFiles();

        if (getValue().empty() && m_items.size() > 0) setValue(0);
    }

    void FilesystemComboComponent::onUpdate() {
        globFiles();
    }

    FilesystemComboComponent* FilesystemComboComponent::callback(const std::function<void(int)>& func) {
        m_callback = func;
        return this;
    }

    std::filesystem::path FilesystemComboComponent::getValue() const {
        return get_value(m_id, std::filesystem::path(), m_noSave);
    }

    const std::vector<std::filesystem::path>& FilesystemComboComponent::getItems() const { return m_items; }

    void FilesystemComboComponent::setValue(std::filesystem::path path) const {
        store_value(m_id, path, m_noSave);
    }

    void FilesystemComboComponent::setValue(int index) const {
        setValue(m_items[index]);
    }

    const std::string& FilesystemComboComponent::getId() const { return m_id; }
    const std::string& FilesystemComboComponent::getTitle() const { return m_title; }
    std::string* FilesystemComboComponent::getSearchBuffer() { return &m_searchBuffer; }

    FilesystemComboComponent* FilesystemComboComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void FilesystemComboComponent::triggerCallback(int value) const {
        if (m_callback) m_callback(value);
    }

    void FilesystemComboComponent::globFiles() {
        m_items.clear();

        std::error_code ec;
        if (!std::filesystem::exists(m_directory, ec)) return;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(m_directory, ec)) {
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

    FloatToggleComponent* FloatToggleComponent::toggleCallback(const std::function<void()>& func) {
        m_toggleCallback = func;
        return this;
    }

    FloatToggleComponent* FloatToggleComponent::valueCallback(const std::function<void(float)>& func) {
        m_valueCallback = func;
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

    const std::string& FloatToggleComponent::getId() const { return m_id; }
    const std::string& FloatToggleComponent::getTitle() const { return m_title; }
    bool FloatToggleComponent::hasKeybind() const { return m_hasKeybind; }
    float FloatToggleComponent::getMin() const { return m_min; }
    float FloatToggleComponent::getMax() const { return m_max; }
    const std::string& FloatToggleComponent::getFormat() const { return m_format; }

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

    void FloatToggleComponent::triggerCallback(float value) const {
        if (m_valueCallback) m_valueCallback(value);
    }

    void FloatToggleComponent::triggerCallback() const {
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

    InputFloatComponent* InputFloatComponent::callback(const std::function<void(float)>& func) {
        m_callback = func;
        return this;
    }

    const std::string& InputFloatComponent::getId() const { return m_id; }
    const std::string& InputFloatComponent::getTitle() const { return m_title; }
    float InputFloatComponent::getMin() const { return m_min; }
    float InputFloatComponent::getMax() const { return m_max; }
    const std::string& InputFloatComponent::getFormat() const { return m_format; }
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

    void InputFloatComponent::triggerCallback(float value) const {
        if (m_callback) m_callback(value);
    }

    #pragma endregion InputFloatComponent

    #pragma region InputIntComponent

    InputIntComponent::InputIntComponent(std::string title, std::string id, int min, int max): m_id(std::move(id)),
        m_title(std::move(title)), m_min(min), m_max(max) {
        m_type = ComponentType::InputInt;
    }

    InputIntComponent* InputIntComponent::callback(const std::function<void(int)>& func) {
        m_callback = func;
        return this;
    }

    const std::string& InputIntComponent::getId() const { return m_id; }
    const std::string& InputIntComponent::getTitle() const { return m_title; }
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

    void InputIntComponent::triggerCallback(int value) const {
        if (m_callback) m_callback(value);
    }

    #pragma endregion InputIntComponent

    #pragma region InputTextComponent

    InputTextComponent::InputTextComponent(std::string title, std::string id): m_id(std::move(id)),
                                                                               m_title(std::move(title)) {
        m_type = ComponentType::InputText;
    }

    InputTextComponent* InputTextComponent::callback(const std::function<void(std::string)>& func) {
        m_callback = func;
        return this;
    }

    const std::string& InputTextComponent::getId() const { return m_id; }
    const std::string& InputTextComponent::getTitle() const { return m_title; }

    std::string InputTextComponent::getValue() const {
        return get_value<std::string>(m_id, "", m_noSave);
    }

    void InputTextComponent::setValue(const std::string& value) const {
        store_value_ref(m_id, value, m_noSave);
    }

    InputTextComponent* InputTextComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void InputTextComponent::triggerCallback(std::string value) const {
        if (m_callback) m_callback(std::move(value));
    }

    #pragma endregion InputTextComponent

    #pragma region IntToggleComponent

    IntToggleComponent::IntToggleComponent(std::string title, std::string id, int min, int max): m_id(std::move(id)),
        m_title(std::move(title)), m_min(min), m_max(max) {
        m_type = ComponentType::IntToggle;
    }

    IntToggleComponent* IntToggleComponent::toggleCallback(const std::function<void()>& func) {
        m_toggleCallback = func;
        return this;
    }

    IntToggleComponent* IntToggleComponent::valueCallback(const std::function<void(int)>& func) {
        m_valueCallback = func;
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

    const std::string& IntToggleComponent::getId() const { return m_id; }
    const std::string& IntToggleComponent::getTitle() const { return m_title; }
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

    void IntToggleComponent::triggerCallback(int value) const {
        if (m_valueCallback) m_valueCallback(value);
    }

    void IntToggleComponent::triggerCallback() const {
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

    const std::string& KeybindComponent::getId() const { return m_id; }
    const std::string& KeybindComponent::getTitle() const { return m_title; }
    bool KeybindComponent::canDelete() const { return m_canDelete; }
    keybinds::Keys KeybindComponent::getDefaultKey() const { return m_defaultKey; }

    KeybindComponent* KeybindComponent::setDescription(std::string description) {
        m_description = std::move(description);
        return this;
    }

    void KeybindComponent::triggerCallback(keybinds::Keys key) const {
        if (m_callback) m_callback(key);
    }

    KeybindComponent* KeybindComponent::callback(const std::function<void(keybinds::Keys)>& func) {
        m_callback = func;
        return this;
    }

    #pragma endregion KeybindComponent

    #pragma region LabelComponent

    LabelComponent::LabelComponent(std::string title): m_title(std::move(title)) {
        m_type = ComponentType::Label;
    }

    void LabelComponent::setText(std::string text) { m_title = std::move(text); }
    const std::string& LabelComponent::getId() const { return m_title; }
    const std::string& LabelComponent::getTitle() const { return m_title; }

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

    const std::string& LabelSettingsComponent::getId() const { return m_id; }
    const std::string& LabelSettingsComponent::getTitle() const { return m_settings->text; }
    labels::LabelSettings* LabelSettingsComponent::getSettings() const { return m_settings; }

    LabelSettingsComponent* LabelSettingsComponent::deleteCallback(const std::function<void()>& func) {
        m_deleteCallback = func;
        return this;
    }

    LabelSettingsComponent* LabelSettingsComponent::editCallback(const std::function<void()>& func) {
        m_editCallback = func;
        return this;
    }

    LabelSettingsComponent* LabelSettingsComponent::moveCallback(const std::function<void(bool)>& func) {
        m_moveCallback = func;
        return this;
    }

    LabelSettingsComponent* LabelSettingsComponent::exportCallback(const std::function<void()>& func) {
        m_exportCallback = func;
        return this;
    }

    void LabelSettingsComponent::triggerDeleteCallback() const {
        if (m_deleteCallback) m_deleteCallback();

        // We also have to clean up the keybind
        keybinds::Manager::get()->unregisterKeybind(fmt::format("label.{}", m_settings->id));
    }

    void LabelSettingsComponent::triggerEditCallback() const {
        if (m_editCallback) m_editCallback();

        // Update the keybind title
        auto keybind = keybinds::Manager::get()->getKeybind(fmt::format("label.{}", m_settings->id));
        if (keybind.has_value()) {
            keybind->get().setTitle(m_settings->name);
        } else {
            geode::log::warn("Keybind with ID 'label.{}' not found", m_settings->id);
        }
    }

    void LabelSettingsComponent::triggerMoveCallback(bool up) const {
        if (m_moveCallback) m_moveCallback(up);
    }

    void LabelSettingsComponent::triggerExportCallback() const {
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

    RadioButtonComponent* RadioButtonComponent::callback(const std::function<void(int)>& func) {
        m_callback = func;
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

    const std::string& RadioButtonComponent::getId() const { return m_id; }
    const std::string& RadioButtonComponent::getTitle() const { return m_title; }
    bool RadioButtonComponent::hasKeybind() const { return m_hasKeybind; }

    void RadioButtonComponent::triggerCallback(int value) const {
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

    SliderComponent* SliderComponent::callback(const std::function<void(float)>& func) {
        m_callback = func;
        return this;
    }

    const std::string& SliderComponent::getId() const { return m_id; }
    const std::string& SliderComponent::getTitle() const { return m_title; }
    float SliderComponent::getMin() const { return m_min; }
    float SliderComponent::getMax() const { return m_max; }
    const std::string& SliderComponent::getFormat() const { return m_format; }

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

    void SliderComponent::triggerCallback(float value) const {
        if (m_callback) m_callback(value);
    }

    #pragma endregion SliderComponent

    #pragma region ToggleComponent

    ToggleComponent::ToggleComponent(std::string id, std::string title)
        : m_id(std::move(id)), m_title(std::move(title)) {
        m_type = ComponentType::Toggle;
    }

    ToggleComponent* ToggleComponent::callback(const std::function<void(bool)>& func) {
        m_callback = func;
        return this;
    }

    void ToggleComponent::addOptions(const std::function<void(std::shared_ptr<MenuTab>)>& options) {
        if (!m_options) m_options = std::make_shared<MenuTab>(m_title, false);

        options(m_options);
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

    const std::string& ToggleComponent::getId() const { return m_id; }
    const std::string& ToggleComponent::getTitle() const { return m_title; }
    std::weak_ptr<MenuTab> ToggleComponent::getOptions() const { return m_options; }
    bool ToggleComponent::hasKeybind() const { return m_hasKeybind; }

    void ToggleComponent::triggerCallback(bool value) const {
        if (m_callback) m_callback(value);
    }

    #pragma endregion ToggleComponent
}
