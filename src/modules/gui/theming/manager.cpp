#include "manager.hpp"
#include <filesystem>
#include <nlohmann/json.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/imgui/imgui.hpp>
#include <Geode/Loader.hpp>
#include <Geode/utils/cocos.hpp>
#include <modules/gui/blur/blur.hpp>

namespace eclipse::gui {
    static auto IMPORT_PICK_OPTIONS = geode::utils::file::FilePickOptions{
        std::nullopt, {{
            "Eclipse Themes",
            {"*.zip"}
        }}
    };

    int getDefaultThemeIndex(std::vector<ThemeMeta> const& themes) {
        #ifdef GEODE_IS_DESKTOP
        static auto defaultPath = geode::Mod::get()->getResourcesDir() / "megaoverlay.json";
        #else
        static auto defaultPath = geode::Mod::get()->getResourcesDir() / "catppuccinmacchiato.json";
        #endif
        for (auto i = 0; i < themes.size(); i++) {
            if (themes[i].path == defaultPath) return i;
        }
        return 0;
    }

    void ThemeManager::init() {
        if (!loadTheme(geode::Mod::get()->getSaveDir() / "theme.json")) {
            // theme not created, load a built-in one
            auto themes = listAvailableThemes();
            if (themes.empty()) {
                // okay, we're screwed, just set the defaults
                applyValues(config::getTempStorage(), true);
                return setDefaults();
            }

            // TODO: add a priority for default theme
            config::setIfEmpty("themeIndex", getDefaultThemeIndex(themes));
            loadTheme(themes[config::get<int>("themeIndex", 0)].path);
        }
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
        m_schemaVersion = THEME_SCHEMA_VERSION;

        m_uiScale = 1.f;
        m_borderSize = 1.f;
        m_windowRounding = 0.f;
        m_frameRounding = 4.f;
        m_windowMargin = 4.f;

        m_enableBlur = true;
        m_blurSpeed = 0.3f;
        m_blurRadius = 1.f;

        m_searchedColor = Color{ 255, 0, 0, 255 };

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

    void ThemeManager::reloadTheme() {
        auto themes = listAvailableThemes();
        if (themes.empty()) {
            applyValues(config::getTempStorage(), true);
            return setDefaults();
        }
        loadTheme(themes[config::get<int>("themeIndex", 0)].path);
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
        else geode::log::warn("Failed to read \"{}\" from theme", key);
    }

    bool ThemeManager::loadTheme(const std::filesystem::path& path) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) return false;
        std::ifstream file(path);
        if (!file.is_open()) return false;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return false;

        setDefaults();
        auto details = json["details"];

        int schemaVersion = 0;
        try_assign(schemaVersion, details, "schema");
        if (schemaVersion != THEME_SCHEMA_VERSION) return false;
        m_schemaVersion = schemaVersion;

        try_assign(m_themeName, details, "name");
        try_assign(m_themeDescription, details, "description");
        try_assign(m_themeAuthor, details, "author");

        auto renderer = json_try_get<int>(details, "renderer");
        auto layout = json_try_get<int>(details, "layout");
        auto theme = json_try_get<int>(details, "style");

        if (renderer) this->setRenderer(static_cast<RendererType>(*renderer));
        if (layout) this->setLayoutMode(static_cast<imgui::LayoutMode>(*layout));
        if (theme) this->setComponentTheme(static_cast<imgui::ComponentTheme>(*theme));

        // force renderer to cocos2d on launch
        #if defined(ECLIPSE_DEBUG_BUILD) && defined(GEODE_IS_MOBILE)
        this->setRenderer(RendererType::Cocos2d);
        #endif

        auto other = json["other"];
        try_assign(m_uiScale, other, "uiScale");
        try_assign(m_selectedFont, other, "font");

        float fontSize = m_fontSize;
        try_assign(fontSize, other, "fontSize");
        this->setFontSize(fontSize);
        if (auto imgui = imgui::ImGuiRenderer::get()) imgui->reload();

        try_assign(m_borderSize, other, "borderSize");
        try_assign(m_framePadding, other, "framePadding");
        try_assign(m_frameRounding, other, "frameRounding");
        try_assign(m_windowMargin, other, "windowMargin");
        try_assign(m_windowRounding, other, "windowRounding");
        try_assign(m_windowPadding, other, "windowPadding");
        try_assign(m_verticalSpacing, other, "verticalSpacing");
        try_assign(m_horizontalSpacing, other, "horizontalSpacing");
        try_assign(m_verticalInnerSpacing, other, "verticalInnerSpacing");
        try_assign(m_horizontalInnerSpacing, other, "horizontalInnerSpacing");
        try_assign(m_indentSpacing, other, "indentSpacing");

        // blur
        auto blur = json["blur"];
        try_assign(m_enableBlur, blur, "blurEnabled");
        try_assign(m_blurSpeed, blur, "blurSpeed");
        try_assign(m_blurRadius, blur, "blurRadius");

        // overrides
        auto colors = json["colors"];
        try_assign(m_backgroundColor, colors, "backgroundColor");
        try_assign(m_foregroundColor, colors, "foregroundColor");
        try_assign(m_frameBackground, colors, "frameBackground");
        try_assign(m_disabledColor, colors, "disabledColor");
        try_assign(m_searchedColor, colors, "searchedColor");
        try_assign(m_borderColor, colors, "borderColor");
        try_assign(m_titleBackgroundColor, colors, "titleBackgroundColor");
        try_assign(m_titleForegroundColor, colors, "titleForegroundColor");
        try_assign(m_checkboxBackgroundColor, colors, "checkboxBackgroundColor");
        try_assign(m_checkboxCheckmarkColor, colors, "checkboxCheckmarkColor");
        try_assign(m_checkboxForegroundColor, colors, "checkboxForegroundColor");
        try_assign(m_buttonBackgroundColor, colors, "buttonBackgroundColor");
        try_assign(m_buttonForegroundColor, colors, "buttonForegroundColor");
        try_assign(m_buttonDisabledColor, colors, "buttonDisabledColor");
        try_assign(m_buttonDisabledForeground, colors, "buttonDisabledForeground");
        try_assign(m_buttonHoveredColor, colors, "buttonHoveredColor");
        try_assign(m_buttonHoveredForeground, colors, "buttonHoveredForeground");
        try_assign(m_buttonActivatedColor, colors, "buttonActivatedColor");
        try_assign(m_buttonActiveForeground, colors, "buttonActiveForeground");

        applyValues(config::getTempStorage(), true);
        return true;
    }

    void ThemeManager::saveTheme(const std::filesystem::path& path) const {
        nlohmann::json json;
        this->applyValues(json);

        auto res = geode::utils::file::writeString(path, json.dump(4));
        if (res.isErr()) {
            geode::log::error("Failed to save theme file: {}", res.unwrapErr());
        }
    }

    void ThemeManager::saveTheme() const {
        saveTheme(geode::Mod::get()->getSaveDir() / "theme.json");
    }

    void ThemeManager::applyValues(nlohmann::json& json, bool flatten) const {
        auto& details = flatten ? json : json["details"];
        auto& blur = flatten ? json : json["blur"];
        auto& other = flatten ? json : json["other"];
        auto& colors = flatten ? json : json["colors"];

        details["name"] = m_themeName;
        details["description"] = m_themeDescription;
        details["author"] = m_themeAuthor;
        details["renderer"] = m_renderer;
        details["layout"] = m_layoutMode;
        details["style"] = m_componentTheme;
        details["schema"] = m_schemaVersion;

        blur["blurEnabled"] = m_enableBlur;
        blur["blurSpeed"] = m_blurSpeed;
        blur["blurRadius"] = m_blurRadius;

        other["uiScale"] = m_uiScale;
        other["font"] = m_selectedFont;
        other["fontSize"] = m_fontSize;
        other["borderSize"] = m_borderSize;
        other["framePadding"] = m_framePadding;
        other["frameRounding"] = m_frameRounding;
        other["windowMargin"] = m_windowMargin;
        other["windowRounding"] = m_windowRounding;
        other["windowPadding"] = m_windowPadding;
        other["verticalSpacing"] = m_verticalSpacing;
        other["horizontalSpacing"] = m_horizontalSpacing;
        other["verticalInnerSpacing"] = m_verticalInnerSpacing;
        other["horizontalInnerSpacing"] = m_horizontalInnerSpacing;
        other["indentSpacing"] = m_indentSpacing;

        colors["backgroundColor"] = m_backgroundColor;
        colors["frameBackground"] = m_frameBackground;
        colors["foregroundColor"] = m_foregroundColor;
        colors["disabledColor"] = m_disabledColor;
        colors["searchedColor"] = m_searchedColor;
        colors["borderColor"] = m_borderColor;
        colors["titleBackgroundColor"] = m_titleBackgroundColor;
        colors["titleForegroundColor"] = m_titleForegroundColor;
        colors["checkboxBackgroundColor"] = m_checkboxBackgroundColor;
        colors["checkboxCheckmarkColor"] = m_checkboxCheckmarkColor;
        colors["checkboxForegroundColor"] = m_checkboxForegroundColor;
        colors["buttonBackgroundColor"] = m_buttonBackgroundColor;
        colors["buttonForegroundColor"] = m_buttonForegroundColor;
        colors["buttonDisabledColor"] = m_buttonDisabledColor;
        colors["buttonDisabledForeground"] = m_buttonDisabledForeground;
        colors["buttonHoveredColor"] = m_buttonHoveredColor;
        colors["buttonHoveredForeground"] = m_buttonHoveredForeground;
        colors["buttonActivatedColor"] = m_buttonActivatedColor;
        colors["buttonActiveForeground"] = m_buttonActiveForeground;

        if (flatten) { // flatten is only ever used for temp config
            json["accent"] = m_titleBackgroundColor;
            json["background"] = m_backgroundColor;
        }
    }

    bool ThemeManager::importTheme(const std::filesystem::path& path) {
        return false;
    }

    void ThemeManager::exportTheme(const std::filesystem::path& path) {}

    float ThemeManager::getGlobalScale() const {
        auto ret = m_uiScale * imgui::DEFAULT_SCALE;
        if (config::get<"interface.dpi-scaling", bool>()) {
            ret *= config::getTemp<"ui.scale", float>(1.f);
        } else {
            GEODE_MACOS(ret /= geode::utils::getDisplayFactor();)
        }
        return ret;
    }

    std::optional<ThemeMeta> ThemeManager::checkTheme(const std::filesystem::path& path) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) return std::nullopt;
        std::ifstream file(path);
        if (!file.is_open()) return std::nullopt;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return std::nullopt;

        auto details = json["details"];
        auto name = json_try_get<std::string>(details, "name");
        if (!name) return std::nullopt;

        return ThemeMeta{name.value(), path};
    }

    std::vector<ThemeMeta> ThemeManager::listAvailableThemes() {
        std::vector<ThemeMeta> themes;
        auto globThemes = [&](std::filesystem::path const& path) {
            std::error_code ec;
            std::filesystem::create_directories(path, ec);
            if (ec) {
                geode::log::warn("Failed to create themes directory {}: {}", path, ec.message());
                return;
            }
            for (auto& entry : std::filesystem::directory_iterator(path, ec)) {
                if (entry.path().extension() != ".json") continue;
                if (auto theme = checkTheme(entry.path()))
                    themes.push_back(*theme);
            }
            if (ec) {
                geode::log::warn("Failed to list themes in {}: {}", path, ec.message());
            }
        };

        globThemes(geode::Mod::get()->getResourcesDir());
        globThemes(geode::Mod::get()->getConfigDir() / "themes");

        return themes;
    }

    void ThemeManager::applyAccentColor(const Color& color) {
        auto isDark = color.luminance() < 0.5f;
        auto foreground = isDark ? Color::WHITE : Color::BLACK;

        m_disabledColor = isDark ? foreground.darken(0.4f) : foreground.lighten(0.4f);
        m_titleBackgroundColor = color;
        m_titleForegroundColor = foreground;
        // m_checkboxCheckmarkColor = color;
        m_buttonBackgroundColor = color;
        m_buttonForegroundColor = foreground;
        m_buttonDisabledColor = isDark ? color.lighten(0.3f) : color.darken(0.3f);
        m_buttonDisabledForeground = isDark ? foreground.darken(0.3f) : foreground.lighten(0.3f);
        m_buttonHoveredColor = isDark ? color.lighten(0.1f) : color.darken(0.1f);
        m_buttonHoveredForeground = foreground;
        m_buttonActivatedColor = isDark ? color.lighten(0.2f) : color.darken(0.2f);
        m_buttonActiveForeground = foreground;
    }

    void ThemeManager::applyBackgroundColor(const Color& color) {
        auto isDark = color.luminance() < 0.5f;
        auto foreground = isDark ? Color::WHITE : Color::BLACK;

        m_backgroundColor = color;
        m_frameBackground = isDark ? color.lighten(0.1f) : color.darken(0.1f);
        m_borderColor = color.lighten(0.2f);
        m_foregroundColor = foreground;
        m_checkboxForegroundColor = foreground;
        m_checkboxBackgroundColor = color.lighten(0.1f);
    }

    void ThemeManager::setRenderer(RendererType renderer) {
        auto engine = Engine::get();
        if (engine->isInitialized()) {
            engine->setRenderer(renderer);
        }
        m_renderer = renderer;
    }

    void ThemeManager::setLayoutMode(imgui::LayoutMode mode) {
        if (auto imgui = imgui::ImGuiRenderer::get()) {
            geode::log::debug("ThemeManager::setLayoutMode - setting new layout");
            imgui->setLayoutMode(mode);
        }
        m_layoutMode = mode;
    }

    void ThemeManager::setComponentTheme(imgui::ComponentTheme theme) {
        if (auto imgui = imgui::ImGuiRenderer::get()) {
            imgui->setComponentTheme(theme);
        }
        m_componentTheme = theme;
    }

    void ThemeManager::setSelectedFont(const std::string& value) {
        if (auto imgui = imgui::ImGuiRenderer::get()) {
            imgui->getFontManager().setFont(value);
        }
        m_selectedFont = value;
    }

    void ThemeManager::setSelectedFont(int index) {
        auto fonts = getFontNames();
        if (fonts.size() <= index) return;
        setSelectedFont(fonts[index]);
    }

    std::vector<std::string> ThemeManager::getFontNames() {
        auto fonts = imgui::FontManager::fetchAvailableFonts();
        std::vector<std::string> result;
        result.reserve(fonts.size());
        for (auto& font : fonts) {
            result.emplace_back(font.getName());
        }
        return result;
    }

    void ThemeManager::setFontSize(float value) {
        m_fontSize = value;
    }

    void ThemeManager::setBlurEnabled(bool value) {
        m_enableBlur = value;
        blur::toggle(value);
    }
}
