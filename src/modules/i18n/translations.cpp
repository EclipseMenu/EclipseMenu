#include "translations.hpp"
#include <nlohmann/json.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/setting.hpp>

namespace eclipse::i18n {

    nlohmann::json g_fallback = {
        {"language-code", "null"},
    };
    nlohmann::json g_translations = {
        {"language-code", "en"},
        {"language-name", "English"}
    };

    std::string_view get(std::string_view key) {
        if (!g_translations.contains(key)) {
            return g_fallback.value(key, key);
        }
        return g_translations.value(key, key);
    }

    std::string get_(std::string_view key) {
        return std::string(get(key));
    }

    // Dirty hack to refresh constant strings
    void refreshStaticArrays() {
        labels::alignmentNames[0] = get("labels.alignment.top-left").data();
        labels::alignmentNames[1] = get("labels.alignment.top-center").data();
        labels::alignmentNames[2] = get("labels.alignment.top-right").data();
        labels::alignmentNames[3] = get("labels.alignment.middle-left").data();
        labels::alignmentNames[4] = get("labels.alignment.middle-center").data();
        labels::alignmentNames[5] = get("labels.alignment.middle-right").data();
        labels::alignmentNames[6] = get("labels.alignment.bottom-left").data();
        labels::alignmentNames[7] = get("labels.alignment.bottom-center").data();
        labels::alignmentNames[8] = get("labels.alignment.bottom-right").data();
    }

    void loadFallback(std::string_view code) {
        auto langs = fetchAvailableLanguages();
        auto it = std::ranges::find_if(langs, [&](auto const& lang) {
            return lang.code == code;
        });

        if (it == langs.end()) return;

        std::ifstream file(it->path);
        if (!file) return;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return;

        g_fallback = json;
    }

    void setLanguage(std::string_view code) {
        auto langs = fetchAvailableLanguages();
        auto it = std::ranges::find_if(langs, [&](auto const& lang) {
            return lang.code == code;
        });

        if (it == langs.end()) return;

        std::ifstream file(it->path);
        if (!file) return;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return;

        g_translations = json;
        config::setTemp("language.index", std::distance(langs.begin(), it));

        // check if current fallback is the same as the new language
        if (g_fallback["language-code"].get<std::string>() != it->fallback)
            loadFallback(it->fallback);

        refreshStaticArrays();
    }

    std::string getCurrentLanguage() {
        if (!g_translations.contains("language-code")) {
            return "en";
        }
        return g_translations["language-code"];
    }

    std::optional<LanguageMetadata> getLanguageName(std::filesystem::path const& path) {
        std::ifstream file(path);
        if (!file) return std::nullopt;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return std::nullopt;

        if (!json.contains("language-name") || !json.contains("language-code")) return std::nullopt;

        auto fallback = json.contains("language-fallback") ? json["language-fallback"].get<std::string>() : "en";

        return LanguageMetadata {
            json["language-name"].get<std::string>(),
            json["language-code"].get<std::string>(),
            fallback, path
        };
    }

    std::vector<LanguageMetadata> fetchAvailableLanguages() {
        static auto langsPath = geode::Mod::get()->getSaveDir() / "languages";
        static auto fallbackPath = geode::Mod::get()->getResourcesDir();

        std::vector<LanguageMetadata> result;
        auto globLangs = [&](std::filesystem::path const& path) {
            std::filesystem::create_directories(path);
            for (auto& entry : std::filesystem::directory_iterator(path)) {
                auto filename = entry.path().filename().string();
                if (filename.size() < 10 || filename.substr(filename.size() - 10) != ".lang.json") continue;
                auto code = filename.substr(0, filename.size() - 10);
                auto name = getLanguageName(entry.path());
                if (!name) continue;
                result.push_back(*name);
            }
        };

        globLangs(langsPath);
        globLangs(fallbackPath);

        return result;
    }

    void downloadLanguages() {

    }

    size_t getLanguageIndex() {
        auto langs = fetchAvailableLanguages();
        auto it = std::ranges::find_if(langs, [&](auto const& lang) {
            return lang.code == config::get<std::string>("language", "en");
        });

        if (it == langs.end()) return 0;
        return std::distance(langs.begin(), it);
    }

    bool operator&(GlyphRange lhs, GlyphRange rhs) {
        return static_cast<int>(lhs) & static_cast<int>(rhs);
    }

    GlyphRange getRequiredGlyphRanges() {
        if (!g_translations.contains("language-charset")) {
            return GlyphRange::Default;
        }

        auto charset = g_translations["language-charset"].get<std::string>();

        if (charset == "greek") return GlyphRange::Greek;
        if (charset == "korean") return GlyphRange::Korean;
        if (charset == "japanese") return GlyphRange::Japanese;
        if (charset == "chinese-full") return GlyphRange::ChineseFull;
        if (charset == "chinese-simplified") return GlyphRange::ChineseSimplified;
        if (charset == "cyrillic") return GlyphRange::Cyrillic;
        if (charset == "thai") return GlyphRange::Thai;
        if (charset == "vietnamese") return GlyphRange::Vietnamese;

        return GlyphRange::Default;
    }

    std::string_view getRequiredGlyphRangesString() {
        if (!g_translations.contains("language-charset")) { return "default"; }
        return g_translations["language-charset"].get<std::string_view>();
    }

    std::vector<std::string> getAvailableLanguages() {
        auto langNames = fetchAvailableLanguages();
        auto currentLang = getCurrentLanguage();
        std::vector<std::string> langNamesVec;
        for (const auto& lang : langNames) {
            if (lang.code == currentLang) {
                langNamesVec.push_back(fmt::format("{} ({})", i18n::get("language-native"), lang.code));
                continue;
            }
            langNamesVec.push_back(fmt::format("{} ({})", lang.name, lang.code));
        }
        return langNamesVec;
    }
}
