#include "manager.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <modules/config/config.hpp>

namespace eclipse::gui {
    void ThemeManager::init() {
        if (!loadTheme(geode::Mod::get()->getSaveDir() / "theme.json")) {
            // theme not created, load a built-in one
            auto themes = listAvailableThemes();
            if (themes.empty()) {
                // okay, we're screwed, just set the defaults
                return setDefaults();
            }

            // TODO: add a priority for default theme
            loadTheme(themes[0].path);
        }

        // load values into temp storage for use with other components
        applyValues(config::getTempStorage());
    }

    void ThemeManager::setDefaults() {
#ifdef GEODE_IS_DESKTOP
        m_renderer = RendererType::ImGui;
        m_layoutMode = imgui::LayoutMode::Tabbed;
#else
        m_renderer = RendererType::Cocos2d;
        m_layoutMode = imgui::LayoutMode::Panel;
#endif

        m_componentTheme = imgui::ComponentTheme::MegaHack;

        m_uiScale = 1.f;
        m_borderSize = 1.f;
        m_windowRounding = 0.f;
        m_frameRounding = 4.f;
        m_windowMargin = 4.f;

        m_enableBlur = true;
        m_blurSpeed = 0.15f;
        m_blurRadius = 1.f;

        // TODO: fill this after all properties are figured out
    }

    std::shared_ptr<ThemeManager> ThemeManager::get() {
        static std::shared_ptr<ThemeManager> instance;
        if (!instance) {
            instance = std::make_shared<ThemeManager>();
            instance->init();
        }
        return instance;
    }

    template <typename T>
    std::optional<T> json_try_get(nlohmann::json const& j, std::string_view key) {
        if (!j.is_object()) return std::nullopt;
        if (!j.contains(key)) return std::nullopt;
        return j.at(key).get<T>();
    }

    template <typename T>
    void try_assign(T& v, nlohmann::json const& j, std::string_view key) {
        auto value = json_try_get<T>(j, key);
        if (value) v = *value;
    }

    bool ThemeManager::loadTheme(const std::filesystem::path& path) {
        if (!std::filesystem::exists(path)) return false;
        std::ifstream file(path);
        if (!file.is_open()) return false;

        auto json = nlohmann::json::parse(file, nullptr);
        if (json.is_discarded()) return false;

        setDefaults();
        try_assign(m_themeName, json, "name");
        try_assign(m_themeDescription, json, "description");
        try_assign(m_themeAuthor, json, "author");

        auto renderer = json_try_get<int>(json, "renderer");
        auto layout = json_try_get<int>(json, "layout");
        auto theme = json_try_get<int>(json, "theme");

        if (renderer) Engine::get()->setRenderer(static_cast<RendererType>(*renderer));
        if (auto imguiRenderer = imgui::ImGuiRenderer::get()) {
            if (layout) imguiRenderer->setLayoutMode(static_cast<imgui::LayoutMode>(*layout));
            if (theme) imguiRenderer->setComponentTheme(static_cast<imgui::ComponentTheme>(*theme));
        } else {
            if (layout) m_layoutMode = static_cast<imgui::LayoutMode>(*layout);
            if (theme) m_componentTheme = static_cast<imgui::ComponentTheme>(*theme);
        }

        try_assign(m_uiScale, json, "uiScale");
        try_assign(m_selectedFont, json, "font");
        try_assign(m_fontSize, json, "fontSize");
        try_assign(m_framePadding, json, "framePadding");
        try_assign(m_windowMargin, json, "windowMargin");
        try_assign(m_windowRounding, json, "windowRounding");
        try_assign(m_frameRounding, json, "frameRounding");
        try_assign(m_borderSize, json, "borderSize");
        try_assign(m_enableBlur, json, "blurEnabled");
        try_assign(m_blurSpeed, json, "blurSpeed");
        try_assign(m_blurRadius, json, "blurRadius");

        try_assign(m_backgroundColor, json, "backgroundColor");
        try_assign(m_foregroundColor, json, "foregroundColor");
        try_assign(m_frameBackground, json, "frameBackground");
        try_assign(m_disabledColor, json, "disabledColor");
        try_assign(m_borderColor, json, "borderColor");
        try_assign(m_titleBackgroundColor, json, "titleBackgroundColor");
        try_assign(m_titleForegroundColor, json, "titleForegroundColor");
        try_assign(m_checkboxBackgroundColor, json, "checkboxBackgroundColor");
        try_assign(m_checkboxCheckmarkColor, json, "checkboxCheckmarkColor");
        try_assign(m_checkboxForegroundColor, json, "checkboxForegroundColor");
        try_assign(m_buttonBackgroundColor, json, "buttonBackgroundColor");
        try_assign(m_buttonForegroundColor, json, "buttonForegroundColor");
        try_assign(m_buttonDisabledColor, json, "buttonDisabledColor");
        try_assign(m_buttonDisabledForeground, json, "buttonDisabledForeground");
        try_assign(m_buttonHoveredColor, json, "buttonHoveredColor");
        try_assign(m_buttonHoveredForeground, json, "buttonSelectedForeground");
        try_assign(m_buttonActivatedColor, json, "buttonActivatedColor");
        try_assign(m_buttonActiveForeground, json, "buttonActiveForeground");

        return true;
    }

    void ThemeManager::saveTheme(const std::filesystem::path& path) const {
        std::ofstream file(path);
        if (!file.is_open()) return;

        nlohmann::json json;
        applyValues(json);

        file << json.dump(4);
        file.close();
    }

    void ThemeManager::saveTheme() const {
        saveTheme(geode::Mod::get()->getSaveDir() / "theme.json");
    }

    void ThemeManager::applyValues(nlohmann::json& json) const {
        json["name"] = m_themeName;
        json["description"] = m_themeDescription;
        json["author"] = m_themeAuthor;
        json["renderer"] = m_renderer;
        json["layout"] = m_layoutMode;
        json["theme"] = m_componentTheme;
        json["uiScale"] = m_uiScale;
        json["font"] = m_selectedFont;
        json["fontSize"] = m_fontSize;
        json["framePadding"] = m_framePadding;
        json["windowMargin"] = m_windowMargin;
        json["windowRounding"] = m_windowRounding;
        json["frameRounding"] = m_frameRounding;
        json["borderSize"] = m_borderSize;
        json["blurEnabled"] = m_enableBlur;
        json["blurSpeed"] = m_blurSpeed;
        json["blurRadius"] = m_blurRadius;
        json["backgroundColor"] = m_backgroundColor;
        json["frameBackground"] = m_frameBackground;
        json["foregroundColor"] = m_foregroundColor;
        json["disabledColor"] = m_disabledColor;
        json["borderColor"] = m_borderColor;
        json["titleBackgroundColor"] = m_titleBackgroundColor;
        json["titleForegroundColor"] = m_titleForegroundColor;
        json["checkboxBackgroundColor"] = m_checkboxBackgroundColor;
        json["checkboxCheckmarkColor"] = m_checkboxCheckmarkColor;
        json["checkboxForegroundColor"] = m_checkboxForegroundColor;
        json["buttonBackgroundColor"] = m_buttonBackgroundColor;
        json["buttonForegroundColor"] = m_buttonForegroundColor;
        json["buttonDisabledColor"] = m_buttonDisabledColor;
        json["buttonDisabledForeground"] = m_buttonDisabledForeground;
        json["buttonHoveredColor"] = m_buttonHoveredColor;
        json["buttonHoveredForeground"] = m_buttonHoveredForeground;
        json["buttonActivatedColor"] = m_buttonActivatedColor;
        json["buttonActiveForeground"] = m_buttonActiveForeground;
    }

    bool ThemeManager::importTheme(const std::filesystem::path& path) {
        return false;
    }

    void ThemeManager::exportTheme(const std::filesystem::path &path) {

    }

    float ThemeManager::getGlobalScale() const {
        return m_uiScale * config::getTemp<float>("ui.scale", 1.f);
    }

    std::optional<ThemeMeta> ThemeManager::checkTheme(const std::filesystem::path &path) {
        if (!std::filesystem::exists(path)) return std::nullopt;
        std::ifstream file(path);
        if (!file.is_open()) return std::nullopt;

        auto json = nlohmann::json::parse(file, nullptr);
        if (json.is_discarded()) return std::nullopt;

        auto name = json_try_get<std::string>(json, "name");
        if (!name) return std::nullopt;

        return ThemeMeta { name.value(), path };
    }

    std::vector<ThemeMeta> ThemeManager::listAvailableThemes() {
        std::vector<ThemeMeta> themes;
        auto globThemes = [&](std::filesystem::path const& path) {
            std::filesystem::create_directories(path);
            for (auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.path().extension() != ".json") continue;
                if (auto theme = checkTheme(entry.path()))
                    themes.push_back(*theme);
            }
        };

        globThemes(geode::Mod::get()->getResourcesDir());
        globThemes(geode::Mod::get()->getConfigDir() / "themes");

        return themes;
    }
}
