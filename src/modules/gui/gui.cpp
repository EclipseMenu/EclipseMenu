#include "gui.hpp"

#include <algorithm>
#include <modules/config/config.hpp>

#include "blur/blur.hpp"
#include "cocos/cocos.hpp"
#include "imgui/imgui.hpp"
#include "theming/manager.hpp"

#include "components/base-component.hpp"
#include "components/button.hpp"
#include "components/color.hpp"
#include "components/combo.hpp"
#include "components/filesystem-combo.hpp"
#include "components/float-toggle.hpp"
#include "components/input-float.hpp"
#include "components/input-int.hpp"
#include "components/input-text.hpp"
#include "components/int-toggle.hpp"
#include "components/keybind.hpp"
#include "components/label-settings.hpp"
#include "components/label.hpp"
#include "components/radio.hpp"
#include "components/slider.hpp"
#include "components/toggle.hpp"

namespace eclipse::gui {
    MenuTab::MenuTab(std::string title, bool isSearchedFor)
        : m_title(std::move(title)), m_isSearchedFor(isSearchedFor) {}

    void MenuTab::addComponent(const std::shared_ptr<Component>& component) {
        m_components.push_back(component);
        component->onInit();
    }

    void MenuTab::removeComponent(std::weak_ptr<Component> component) {
        auto it = std::ranges::find_if(
            m_components, [&component](const std::shared_ptr<Component>& c) {
                return component.lock() == c;
            }
        );

        m_components[it - m_components.begin()].reset();
        m_components.erase(it);
    }

    std::shared_ptr<LabelComponent> MenuTab::addLabel(const std::string& title) {
        auto label = std::make_shared<LabelComponent>(title);
        addComponent(label);
        return label;
    }

    std::shared_ptr<ToggleComponent> MenuTab::addToggle(const std::string& id) {
        auto toggle = std::make_shared<ToggleComponent>(id, id);
        addComponent(toggle);
        return toggle;
    }

    std::shared_ptr<ToggleComponent> MenuTab::addToggle(const std::string& title, const std::string& id) {
        auto toggle = std::make_shared<ToggleComponent>(id, title);
        addComponent(toggle);
        return toggle;
    }

    std::shared_ptr<RadioButtonComponent> MenuTab::addRadioButton(
        const std::string& title, const std::string& id, int value
    ) {
        auto button = std::make_shared<RadioButtonComponent>(id, title, value);
        addComponent(button);
        return button;
    }

    std::shared_ptr<ComboComponent> MenuTab::addCombo(
        const std::string& title, const std::string& id, std::vector<std::string> items, int value
    ) {
        auto combo = std::make_shared<ComboComponent>(id, title, items, value);
        addComponent(combo);
        return combo;
    }

    std::shared_ptr<ComboComponent> MenuTab::addCombo(
        const std::string& id, std::vector<std::string> items, int value
    ) {
        auto combo = std::make_shared<ComboComponent>(id, id, items, value);
        addComponent(combo);
        return combo;
    }

    std::shared_ptr<FilesystemComboComponent> MenuTab::addFilesystemCombo(
        const std::string& title, const std::string& id, std::filesystem::path directory
    ) {
        auto combo = std::make_shared<FilesystemComboComponent>(id, title, directory);
        addComponent(combo);
        return combo;
    }

    std::shared_ptr<FilesystemComboComponent> MenuTab::addFilesystemCombo(
        const std::string& id, std::filesystem::path directory
    ) {
        auto combo = std::make_shared<FilesystemComboComponent>(id, id, directory);
        addComponent(combo);
        return combo;
    }

    std::shared_ptr<SliderComponent> MenuTab::addSlider(
        const std::string& title, const std::string& id, float min, float max, const std::string& format
    ) {
        auto slider = std::make_shared<SliderComponent>(title, id, min, max, format);
        addComponent(slider);
        return slider;
    }

    std::shared_ptr<SliderComponent> MenuTab::addSlider(
        const std::string& id, float min, float max, const std::string& format
    ) {
        auto slider = std::make_shared<SliderComponent>(id, id, min, max, format);
        addComponent(slider);
        return slider;
    }

    std::shared_ptr<InputFloatComponent> MenuTab::addInputFloat(
        const std::string& title, const std::string& id, float min, float max, const std::string& format
    ) {
        auto inputFloat = std::make_shared<InputFloatComponent>(title, id, min, max, format);
        addComponent(inputFloat);
        return inputFloat;
    }

    std::shared_ptr<InputFloatComponent> MenuTab::addInputFloat(
        const std::string& id, float min, float max, const std::string& format
    ) {
        auto inputFloat = std::make_shared<InputFloatComponent>(id, id, min, max, format);
        addComponent(inputFloat);
        return inputFloat;
    }

    std::shared_ptr<InputIntComponent> MenuTab::addInputInt(
        const std::string& title, const std::string& id, int min, int max
    ) {
        auto inputInt = std::make_shared<InputIntComponent>(title, id, min, max);
        addComponent(inputInt);
        return inputInt;
    }

    std::shared_ptr<InputIntComponent> MenuTab::addInputInt(const std::string& id, int min, int max) {
        auto inputInt = std::make_shared<InputIntComponent>(id, id, min, max);
        addComponent(inputInt);
        return inputInt;
    }

    std::shared_ptr<IntToggleComponent> MenuTab::addIntToggle(
        const std::string& title, const std::string& id, int min, int max
    ) {
        auto intToggle = std::make_shared<IntToggleComponent>(title, id, min, max);
        addComponent(intToggle);
        return intToggle;
    }

    std::shared_ptr<IntToggleComponent> MenuTab::addIntToggle(const std::string& id, int min, int max) {
        auto intToggle = std::make_shared<IntToggleComponent>(id, id, min, max);
        addComponent(intToggle);
        return intToggle;
    }

    std::shared_ptr<FloatToggleComponent> MenuTab::addFloatToggle(
        const std::string& title, const std::string& id, float min, float max, const std::string& format
    ) {
        auto floatToggle = std::make_shared<FloatToggleComponent>(title, id, min, max, format);
        addComponent(floatToggle);
        return floatToggle;
    }

    std::shared_ptr<FloatToggleComponent> MenuTab::addFloatToggle(
        const std::string& id, float min, float max, const std::string& format
    ) {
        auto floatToggle = std::make_shared<FloatToggleComponent>(id, id, min, max, format);
        addComponent(floatToggle);
        return floatToggle;
    }

    std::shared_ptr<InputTextComponent> MenuTab::addInputText(const std::string& title, const std::string& id) {
        auto inputText = std::make_shared<InputTextComponent>(title, id);
        addComponent(inputText);
        return inputText;
    }

    std::shared_ptr<InputTextComponent> MenuTab::addInputText(const std::string& id) {
        auto inputText = std::make_shared<InputTextComponent>(id, id);
        addComponent(inputText);
        return inputText;
    }

    std::shared_ptr<ButtonComponent> MenuTab::addButton(const std::string& title) {
        auto button = std::make_shared<ButtonComponent>(title);
        addComponent(button);
        return button;
    }

    std::shared_ptr<ColorComponent> MenuTab::addColorComponent(
        const std::string& title, const std::string& id, bool hasOpacity
    ) {
        auto color = std::make_shared<ColorComponent>(title, id, hasOpacity);
        addComponent(color);
        return color;
    }

    std::shared_ptr<ColorComponent> MenuTab::addColorComponent(const std::string& id, bool hasOpacity) {
        auto color = std::make_shared<ColorComponent>(id, id, hasOpacity);
        addComponent(color);
        return color;
    }

    std::shared_ptr<KeybindComponent> MenuTab::addKeybind(
        const std::string& title, const std::string& id, bool canDelete
    ) {
        auto keybind = std::make_shared<KeybindComponent>(title, id, canDelete);
        addComponent(keybind);
        return keybind;
    }

    std::shared_ptr<LabelSettingsComponent> MenuTab::addLabelSetting(labels::LabelSettings* settings) {
        auto labelSettings = std::make_shared<LabelSettingsComponent>(settings);
        addComponent(labelSettings);
        return labelSettings;
    }

    std::shared_ptr<MenuTab> MenuTab::find(std::string_view name) { return Engine::get()->findTab(name); }

    void Engine::setRenderer(RendererType type) {
        // technical debt: think about how to better handle renderer switching
        type = geode::getMod()->getSettingValue<std::string>("menu-style") == "ImGui"
            ? RendererType::ImGui
            : RendererType::Cocos2d;

        auto tm = ThemeManager::get();
        if (m_renderer && type == m_renderer->getType()) return;
        if (m_renderer) m_renderer->shutdown();

        switch (type) {
            default:
            case RendererType::ImGui:
                m_renderer = std::make_shared<imgui::ImGuiRenderer>();
                break;
            case RendererType::Cocos2d:
                m_renderer = std::make_shared<cocos::CocosRenderer>();
                break;
        }

        m_renderer->init();

        // update blur state
        blur::toggle(
            config::getTemp("blurEnabled", false)
            && type != RendererType::Cocos2d
        );
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
        auto tab = std::make_shared<MenuTab>(std::string(name), false);
        m_tabs.push_back(tab);

        // Make sure built-in tabs are sorted in a specific way and come first
        static constexpr std::array<std::string_view, 11> builtInTabs = {
            "tab.global", "tab.level", "tab.bypass", "tab.player", "tab.bot", "tab.creator",
            "tab.labels", "tab.shortcuts", "tab.keybinds", "tab.interface", "tab.recorder"
        };

        std::ranges::sort(m_tabs, [](const auto& a, const auto& b) {
            auto aIt = std::ranges::find(builtInTabs, a->getTitle());
            auto bIt = std::ranges::find(builtInTabs, b->getTitle());
            if (aIt != builtInTabs.end() && bIt != builtInTabs.end()) {
                return aIt - builtInTabs.begin() < bIt - builtInTabs.begin();
            }

            if (aIt != builtInTabs.end()) return true;
            if (bIt != builtInTabs.end()) return false;

            return a->getTitle() < b->getTitle();
        });

        // tell the renderer to update the tabs if we're past the initialization stage
        if (m_initialized && m_renderer) m_renderer->updateTabs();

        return tab;
    }
}
