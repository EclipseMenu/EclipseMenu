#include "gui.hpp"

#include <modules/config/config.hpp>
#include <algorithm>

#include "imgui/imgui.hpp"
#include "cocos/cocos.hpp"
#include "theming/manager.hpp"

namespace eclipse::gui {

    template <typename T>
    T get_value(std::string_view key, T defaultValue, bool useTemp) {
        return useTemp ? config::getTemp<T>(key, defaultValue)
            : config::get<T>(key, defaultValue);
    }

    template <typename T>
    void store_value(std::string_view key, T value, bool useTemp) {
        useTemp ? config::setTemp<T>(key, value)
            : config::set<T>(key, value);
    }

    template <typename T>
    void store_value_ref(std::string_view key, T const& value, bool useTemp) {
        useTemp ? config::setTemp<T>(key, value)
            : config::set<T>(key, value);
    }

    bool ToggleComponent::getValue() const {
        return get_value(m_id, false, m_noSave);
    }

    void ToggleComponent::setValue(bool value) const {
        store_value(m_id, value, m_noSave);
    }

    void ToggleComponent::addOptions(const std::function<void(std::shared_ptr<MenuTab>)>& options) {
        if (!m_options)
            m_options = std::make_shared<MenuTab>(m_title);

        options(m_options);
    }

    ToggleComponent* ToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(m_id, m_title, [this](){
            bool value = !getValue();
            setValue(value);
            this->triggerCallback(value);
        });
        m_hasKeybind = true;
        return this;
    }

    int RadioButtonComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    void RadioButtonComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    RadioButtonComponent* RadioButtonComponent::handleKeybinds() {
        auto specialId = fmt::format("{}-{}", m_id, m_value);
        keybinds::Manager::get()->registerKeybind(specialId, m_title, [this](){
            auto value = getChoice();
            setValue(value);
            this->triggerCallback(value);
        });
        m_hasKeybind = true;
        return this;
    }

    int ComboComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    void ComboComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }


    void ComboComponent::setValueIfEmpty(int value) const {
        if(!config::has(m_id))
            store_value(m_id, value, m_noSave);
    }

    std::filesystem::path FilesystemComboComponent::getValue() const {
        return get_value(m_id, std::filesystem::path(), m_noSave);
    }

    void FilesystemComboComponent::setValue(std::filesystem::path path) const {
        store_value(m_id, path, m_noSave);
    }

    void FilesystemComboComponent::setValue(int index) {
        setValue(m_items[index]);
    }

    void FilesystemComboComponent::globFiles() {
        m_items.clear();

        if(!std::filesystem::exists(m_directory))
            return;

        for (const auto& entry : std::filesystem::recursive_directory_iterator(m_directory))
            m_items.push_back(entry.path());
    }

    float SliderComponent::getValue() const {
        return get_value(m_id, 0.f, m_noSave);
    }

    void SliderComponent::setValue(float value) const {
        store_value(m_id, value, m_noSave);
    }

    float InputFloatComponent::getValue() const {
        return get_value(m_id, 0.f, m_noSave);
    }

    void InputFloatComponent::setValue(float value) const {
        store_value(m_id, value, m_noSave);
    }

    int InputIntComponent::getValue() const {
        return get_value(m_id, 0, m_noSave);
    }

    void InputIntComponent::setValue(int value) const {
        store_value(m_id, value, m_noSave);
    }

    IntToggleComponent* IntToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(m_id, m_title, [this](){
            bool value = !config::get<bool>(fmt::format("{}.toggle", this->getId()), false);
            auto id = fmt::format("{}.toggle", this->getId());
            m_noSave ? config::setTemp(id, value)
                : config::set(id, value);
            this->triggerCallback();
        });
        m_hasKeybind = true;
        return this;
    }

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

    FloatToggleComponent* FloatToggleComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(m_id, m_title, [this](){
            bool value = !config::get<bool>(fmt::format("{}.toggle", this->getId()), false);
            auto id = fmt::format("{}.toggle", this->getId());
            m_noSave ? config::setTemp(id, value)
                : config::set(id, value);
            this->triggerCallback();
        });
        m_hasKeybind = true;
        return this;
    }

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

    std::string InputTextComponent::getValue() const {
        return get_value<std::string>(m_id, "", m_noSave);
    }

    void InputTextComponent::setValue(const std::string& value) const {
        store_value_ref(m_id, value, m_noSave);
    }

    Color ColorComponent::getValue() const {
        return get_value(m_id, Color::BLACK, m_noSave);
    }

    void ColorComponent::setValue(const Color& value) const {
        store_value_ref(m_id, value, m_noSave);
    }

    ButtonComponent* ButtonComponent::handleKeybinds() {
        keybinds::Manager::get()->registerKeybind(fmt::format("button.{}", m_title), m_title, [this](){
            this->triggerCallback();
        });
        m_hasKeybind = true;
        return this;
    }

    void MenuTab::addComponent(const std::shared_ptr<Component>& component) {
        m_components.push_back(component);
        component->onInit();
    }

    void MenuTab::removeComponent(std::weak_ptr<Component> component) {
        auto it = std::ranges::find_if(m_components, [&component](const std::shared_ptr<Component>& c) {
            return component.lock() == c;
        });

        m_components[it - m_components.begin()].reset();
        m_components.erase(it);
    }

    std::shared_ptr<MenuTab> MenuTab::find(std::string_view name) {
        return Engine::get()->findTab(name);
    }

    void Engine::setRenderer(RendererType type) {
        const auto tm = ThemeManager::get();
        if (m_renderer && type == m_renderer->getType()) return;
        if (m_renderer) m_renderer->shutdown();

        switch (type) {
            default:
#ifndef GEODE_IS_MOBILE
            case RendererType::ImGui:
                m_renderer = std::make_shared<imgui::ImGuiRenderer>();
                break;
#endif
            case RendererType::Cocos2d:
                m_renderer = std::make_shared<cocos::CocosRenderer>();
                break;
        }

        m_renderer->init();
    }

    RendererType Engine::getRendererType() {
        auto engine = Engine::get();
        if (!engine->isInitialized()) return RendererType::None;
        if (!engine->m_renderer) return RendererType::None;
        return engine->m_renderer->getType();
    }

    std::shared_ptr<Engine> Engine::get() {
        static auto s_engine = std::make_shared<Engine>();
        return s_engine;
    }

    void Engine::init() {
        m_initialized = true;
        setRenderer(ThemeManager::get()->getRenderer());
    }

    void Engine::toggle() const {
        if (!m_renderer) return;
        m_renderer->toggle();
    }

    std::shared_ptr<MenuTab> Engine::findTab(std::string_view name) {
        for (auto tab : m_tabs) {
            if (tab->getTitle() == name) {
                return tab;
            }
        }

        // If the tab does not exist, create a new one.
        auto tab = std::make_shared<MenuTab>(std::string(name));
        m_tabs.push_back(tab);

        return tab;
    }

}
