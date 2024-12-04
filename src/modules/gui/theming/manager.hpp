#pragma once
#include <memory>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/imgui/imgui.hpp>

#define PROPERTY_CS(type, member, name) private: type member; \
public: type get##name() const { return member; }

#define CR_PROPERTY_CS(type, member, name) private: type member; \
public: type const& get##name() const { return member; }

#define PROPERTY(type, member, name) PROPERTY_CS(type, member, name) \
void set##name(type value) { member = value; }

#define CR_PROPERTY(type, member, name) CR_PROPERTY_CS(type, member, name) \
void set##name(type const& value) { member = value; }

#define COLOR_PROPERTY(member, name) CR_PROPERTY(Color, member, name)

namespace eclipse::gui {

    constexpr auto THEME_SCHEMA_VERSION = 1;

    struct ThemeMeta {
        std::string name;
        std::filesystem::path path;
    };

    /// @brief Holds current theme configuration and manages saving/loading themes.
    class ThemeManager {
        /// @brief Loads current theme/default values
        void init();

        /// @brief Sets all values to default ones. Used to ensure nothing is left uninitialized when loading a theme.
        void setDefaults();

    /// === Saving/loading
    public:

        /// @brief Get the theme manager instance
        static std::shared_ptr<ThemeManager> get();

        /// @brief Reloads the currently selected theme
        void reloadTheme();

        /// @brief Loads a theme from JSON file
        /// @return True if load was successful, or False in case an error happened
        bool loadTheme(const std::filesystem::path& path);

        /// @brief Saves current theme settings to a JSON file
        void saveTheme(const std::filesystem::path& path) const;

        /// @brief Saves current theme to default directory
        void saveTheme() const;

        /// @brief Stores theme values to a json object
        void applyValues(nlohmann::json& json, bool flatten = false) const;

        /// @brief Imports a ZIP file with theme settings and fonts
        /// @return True if import was successful, or False in case an error happened
        bool importTheme(const std::filesystem::path& path);

        /// @brief Exports a ZIP file with theme settings and fonts
        void exportTheme(const std::filesystem::path& path);

    /// === Utils

        /// @brief [ImGui] Gets the actual UI scale (user setting * display ratio)
        float getGlobalScale() const;

        /// @brief Tries to read a theme file and gets its name if successful
        static std::optional<ThemeMeta> checkTheme(const std::filesystem::path& path);
        /// @brief Returns a vector of all available built-in and custom themes
        static std::vector<ThemeMeta> listAvailableThemes();

    /// === Meta-colors

        /// @brief Calculates colors based on the accent color for:
        /// m_disabledColor, m_titleBackgroundColor,
        /// m_titleForegroundColor, m_checkboxCheckmarkColor,
        /// m_buttonBackgroundColor, m_buttonForegroundColor,
        /// m_buttonDisabledColor, m_buttonDisabledForeground,
        /// m_buttonHoveredColor, m_buttonHoveredForeground,
        /// m_buttonActivatedColor, m_buttonActiveForeground
        void applyAccentColor(const Color& color);
        /// @brief Calculates colors based on the background color for:
        /// m_backgroundColor, m_frameBackground, m_borderColor,
        /// m_foregroundColor, m_checkboxForegroundColor, m_checkboxBackgroundColor
        void applyBackgroundColor(const Color& color);

    /// === Properties

        /// [Meta] Theme name
        CR_PROPERTY(std::string, m_themeName, ThemeName)
        /// [Meta] Theme description
        CR_PROPERTY(std::string, m_themeDescription, ThemeDescription)
        /// [Meta] Theme author
        CR_PROPERTY(std::string, m_themeAuthor, ThemeAuthor)
        /// [Meta] Schema version
        CR_PROPERTY(int, m_schemaVersion, SchemaVersion)

        /// Current renderer engine
        PROPERTY_CS(RendererType, m_renderer, Renderer)
        void setRenderer(RendererType renderer);

        /// [ImGui] Layout mode
        PROPERTY_CS(imgui::LayoutMode, m_layoutMode, LayoutMode)
        void setLayoutMode(imgui::LayoutMode mode);
        /// [ImGui] Component theme
        PROPERTY_CS(imgui::ComponentTheme, m_componentTheme, ComponentTheme)
        void setComponentTheme(imgui::ComponentTheme theme);

        /// [ImGui] Global UI Scale
        PROPERTY(float, m_uiScale, UIScale)

        /// [ImGui] Font file name
        CR_PROPERTY_CS(std::string, m_selectedFont, SelectedFont)
        void setSelectedFont(const std::string& value);
        void setSelectedFont(int index);
        static std::vector<std::string> getFontNames();
        /// [ImGui] Font size
        PROPERTY_CS(float, m_fontSize, FontSize)
        void setFontSize(float value);

        /// Frame padding
        PROPERTY(float, m_framePadding, FramePadding)
        /// Frame rounding in px
        PROPERTY(float, m_frameRounding, FrameRounding)

        /// Window margin (also used for stacking distance)
        PROPERTY(float, m_windowMargin, WindowMargin)
        /// Window rounding in px
        PROPERTY(float, m_windowRounding, WindowRounding)
        /// Window padding in px
        PROPERTY(float, m_windowPadding, WindowPadding)

        /// Vertical spacing between components in px
        PROPERTY(float, m_verticalSpacing, VerticalSpacing)
        /// Horizontal spacing between components in px
        PROPERTY(float, m_horizontalSpacing, HorizontalSpacing)
        /// Vertical inner spacing between components in px
        PROPERTY(float, m_verticalInnerSpacing, VerticalInnerSpacing)
        /// Horizontal inner spacing between components in px
        PROPERTY(float, m_horizontalInnerSpacing, HorizontalInnerSpacing)

        /// Indent spacing in px
        PROPERTY(float, m_indentSpacing, IndentSpacing)

        /// Border size in px
        PROPERTY(float, m_borderSize, BorderSize)

        /// Whether to use blur
        PROPERTY(bool, m_enableBlur, BlurEnabled)
        /// Blur animation speed
        PROPERTY(float, m_blurSpeed, BlurSpeed)
        /// Blur intensity
        PROPERTY(float, m_blurRadius, BlurRadius)

        /// Window and popup background color
        COLOR_PROPERTY(m_backgroundColor, BackgroundColor)
        /// Controls background color
        COLOR_PROPERTY(m_frameBackground, FrameBackground)
        /// Primary text color
        COLOR_PROPERTY(m_foregroundColor, ForegroundColor)
        /// Disabled text color
        COLOR_PROPERTY(m_disabledColor, DisabledColor)
        /// Searched text color
        COLOR_PROPERTY(m_searchedColor, SearchedColor)
        /// Primary border color
        COLOR_PROPERTY(m_borderColor, BorderColor)

        /// Title bar background color
        COLOR_PROPERTY(m_titleBackgroundColor, TitleBackgroundColor)
        /// Title bar foreground color
        COLOR_PROPERTY(m_titleForegroundColor, TitleForegroundColor)

        /// Checkbox background color
        COLOR_PROPERTY(m_checkboxBackgroundColor, CheckboxBackgroundColor)
        /// Checkbox checkmark color
        COLOR_PROPERTY(m_checkboxCheckmarkColor, CheckboxCheckmarkColor)
        /// Checkbox foreground color
        COLOR_PROPERTY(m_checkboxForegroundColor, CheckboxForegroundColor)

        /// Button primary background
        COLOR_PROPERTY(m_buttonBackgroundColor, ButtonBackgroundColor)
        /// Button primary foreground
        COLOR_PROPERTY(m_buttonForegroundColor, ButtonForegroundColor)

        /// Button disabled background
        COLOR_PROPERTY(m_buttonDisabledColor, ButtonDisabledBackground)
        /// Button disabled foreground
        COLOR_PROPERTY(m_buttonDisabledForeground, ButtonDisabledForeground)

        /// Button hovered background
        COLOR_PROPERTY(m_buttonHoveredColor, ButtonHoveredBackground)
        /// Button hovered foreground
        COLOR_PROPERTY(m_buttonHoveredForeground, ButtonHoveredForeground)

        /// Button pressed background
        COLOR_PROPERTY(m_buttonActivatedColor, ButtonActivatedBackground)
        /// Button pressed foreground
        COLOR_PROPERTY(m_buttonActiveForeground, ButtonActivatedForeground)

    };

}

#undef PROPERTY
#undef CR_PROPERTY
#undef COLOR_PROPERTY