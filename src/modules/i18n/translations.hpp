#pragma once
#include <string_view>

namespace eclipse::i18n {

    struct LanguageMetadata {
        std::string name;
        std::string code;
        std::string fallback;
        std::string charset;
        std::filesystem::path path;
    };

    /// @brief Get a translation for the specified key.
    std::string_view get(std::string_view key);

    /// @brief Get a translation for the specified key. (std::string version)
    std::string get_(std::string_view key);

    /// @brief Format a translation with the specified arguments.
    /// Note: use numbers in the format string to specify the argument index.
    template <typename... Args>
    std::string format(std::string_view key, Args&&... args) {
        return fmt::format(fmt::runtime(get(key)), std::forward<Args>(args)...);
    }

    /// @brief Loads translations from the specified language file.
    void setLanguage(std::string_view code);

    /// @brief Get the currently selected language code.
    std::string getCurrentLanguage();

    /// @brief Fetch all currently available languages.
    std::vector<LanguageMetadata> fetchAvailableLanguages();

    constexpr std::array<std::string_view, 6> BitmapFontsExtensions = {
        ".fnt", ".png",
        "-hd.fnt", "-hd.png",
        "-uhd.fnt", "-uhd.png"
    };

    /// @brief Check if bitmap fonts are installed for the specified font.
    bool hasBitmapFont(std::string_view font);

    /// @brief Redownload all language files from the server.
    /// Makes sure translations are up-to-date. (does a check once per day)
    void downloadLanguages();

    /// @brief Get the index of the currently selected language.
    size_t getLanguageIndex();

    enum class GlyphRange : uint8_t {
        Default = 0,
        Greek = 1 << 0,
        Korean = 1 << 1,
        Japanese = 1 << 2,
        ChineseFull = 1 << 3,
        ChineseSimplified = 1 << 4,
        Cyrillic = 1 << 5,
        Thai = 1 << 6,
        Vietnamese = 1 << 7
    };

    bool operator&(GlyphRange lhs, GlyphRange rhs);

    /// @brief Get the required glyph ranges for the current language.
    GlyphRange getRequiredGlyphRanges();

    /// @brief Get the required glyph ranges for the current language as a string.
    std::string_view getRequiredGlyphRangesString();

    /// @brief Get a list of names of all available languages.
    std::vector<std::string> getAvailableLanguages();

}