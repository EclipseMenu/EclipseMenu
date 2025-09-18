#include "translations.hpp"

#include <Geode/utils/web.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/setting.hpp>
#include <nlohmann/json.hpp>
#include <utils.hpp>

namespace eclipse::i18n {
    nlohmann::json g_fallback = {
        {"language-code", "null"},
    };
    nlohmann::json g_translations = {
        {"language-code", "en_US"},
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

    static std::string stringExtension(std::filesystem::path const& filename) {
        // turns "en_US.lang.json" into "en_US"
        auto filenameStr = geode::utils::string::pathToString(filename);
        auto pos = filenameStr.find_first_of('.');
        return filenameStr.substr(0, pos);
    }

    static void updateLanguageCode(nlohmann::json& json, std::filesystem::path const& filename) {
        json["language-code"] = stringExtension(filename);
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

        labels::fontAlignmentNames[0] = get("labels.font-alignment.left").data();
        labels::fontAlignmentNames[1] = get("labels.font-alignment.center").data();
        labels::fontAlignmentNames[2] = get("labels.font-alignment.right").data();

        for (size_t i = 0; i < labels::eventNames.size(); ++i) {
            labels::eventNames[i] = get(fmt::format("labels.events.types.{}", i)).data();
        }

        for (size_t i = 0; i < labels::visibleNames.size(); ++i) {
            labels::visibleNames[i] = get(fmt::format("labels.events.visible.{}", i)).data();
        }
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
        updateLanguageCode(g_fallback, it->path.filename());
    }

    bool setLanguage(std::string_view code) {
        auto langs = fetchAvailableLanguages();
        auto it = std::ranges::find_if(langs, [&](auto const& lang) {
            return lang.code == code;
        });

        if (it == langs.end()) return false;

        std::ifstream file(it->path);
        if (!file) return false;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return false;

        g_translations = json;
        updateLanguageCode(g_translations, it->path.filename());
        config::setTemp<uint64_t>("language.index", std::distance(langs.begin(), it));

        // check if current fallback is the same as the new language
        if (g_fallback["language-code"].get<std::string>() != it->fallback)
            loadFallback(it->fallback);

        refreshStaticArrays();
        return true;
    }

    void init() {
        auto language = config::get<std::string_view>("language", DEFAULT_LANGUAGE);
        if (!setLanguage(language)) {
            // fallback to English
            setLanguage(DEFAULT_LANGUAGE);
        }

        downloadLanguages();
    }

    inline time_t getLastCheckTimestamp() {
        return geode::Mod::get()->getSavedValue<int64_t>("last-language-check", ECLIPSE_TRANSLATION_TIMESTAMP);
    }

    inline void setLastCheckTimestamp(time_t timestamp = utils::getTimestamp<utils::seconds>()) {
        geode::Mod::get()->setSavedValue<int64_t>("last-language-check", timestamp);
    }

    std::string getCurrentLanguage() {
        if (!g_translations.contains("language-code")) {
            return DEFAULT_LANGUAGE;
        }
        return g_translations["language-code"];
    }

    std::optional<LanguageMetadata> getLanguageName(std::filesystem::path const& path) {
        std::ifstream file(path);
        if (!file) return std::nullopt;

        auto json = nlohmann::json::parse(file, nullptr, false);
        if (json.is_discarded()) return std::nullopt;

        if (!json.contains("language-name") || !json.contains("language-native")) return std::nullopt;

        auto fallback = json.contains("language-fallback") ? json["language-fallback"].get<std::string>() : DEFAULT_LANGUAGE;
        auto charset = json.contains("language-charset") ? json["language-charset"].get<std::string>() : "default";

        return LanguageMetadata{
            json["language-name"].get<std::string>(),
            std::string(stringExtension(path.filename())),
            fallback,
            charset,
            path
        };
    }

    std::vector<LanguageMetadata> fetchAvailableLanguages() {
        static auto langsPath = geode::Mod::get()->getSaveDir() / "languages";
        static auto fallbackPath = geode::Mod::get()->getResourcesDir();

        std::vector<LanguageMetadata> result;
        auto globLangs = [&](std::filesystem::path const& path) {
            std::error_code ec;
            std::filesystem::create_directories(path, ec);
            if (ec) {
                geode::log::warn("Failed to create languages directory {}: {}", path, ec.message());
                return;
            }
            for (auto& entry : std::filesystem::directory_iterator(path, ec)) {
                auto filename = geode::utils::string::pathToString(entry.path().filename());
                if (filename.size() < 10 || filename.substr(filename.size() - 10) != ".lang.json") continue;

                auto code = filename.substr(0, filename.size() - 10);
                auto name = getLanguageName(entry.path());
                if (!name) continue;

                // filter out duplicates
                if (std::ranges::find_if(result, [&](auto const& lang) {
                    return lang.code == code;
                }) != result.end()) continue;

                result.push_back(std::move(*name));
            }
            if (ec) {
                geode::log::warn("Failed to list languages in {}: {}", path, ec.message());
            }
        };

        globLangs(langsPath);
        globLangs(fallbackPath);

        return result;
    }

    bool hasBitmapFont(std::string_view font) {
        if (font == "default") return true;

        static auto fontsPath = geode::Mod::get()->getConfigDir() / "bmfonts" / GEODE_MOD_ID;
        std::error_code ec;
        if (!std::filesystem::exists(fontsPath, ec))
            return false;

        if (!std::filesystem::exists(fontsPath / fmt::format("font_{}.fnt", font), ec))
            return false;

        return true;
    }

    void downloadLanguage(std::string const& code) {
        static auto langsPath = geode::Mod::get()->getSaveDir() / "languages";

        using namespace geode::utils;
        web::WebRequest()
            .get(fmt::format("https://raw.githubusercontent.com/EclipseMenu/translations/refs/heads/main/translations/{}.lang.json", code))
            .listen([code](web::WebResponse* value) {
                if (!value->ok()) {
                    return;
                }

                auto content = value->string().unwrapOrDefault();
                if (content.empty()) {
                    geode::log::warn("Failed to download language file: {}", code);
                    return;
                }

                std::error_code ec;
                std::filesystem::create_directories(langsPath, ec);
                if (ec) {
                    geode::log::warn("Failed to create languages directory: {}", ec.message());
                    return;
                }

                auto path = langsPath / fmt::format("{}.lang.json", code);
                auto res = file::writeString(path, content);
                if (!res) {
                    geode::log::warn("Failed to save language file: {}", path);
                    return;
                }

                geode::log::debug("Downloaded language file: {}", code);
            });
    }

    geode::Result<> handleLanguageMetadata(matjson::Value metadata) {
        auto lastCheck = getLastCheckTimestamp();

        GEODE_UNWRAP_INTO(auto metadataTimestamp, metadata["timestamp"].asUInt());
        if (metadataTimestamp <= lastCheck) return geode::Ok();

        // languages is an array of locale names
        GEODE_UNWRAP_INTO(auto languages, metadata["languages"].asArray());
        for (auto const& lang : languages) {
            GEODE_UNWRAP_INTO(auto code, lang.asString());
            downloadLanguage(code);
        }

        setLastCheckTimestamp();
        return geode::Ok();
    }

    void downloadLanguages() {
        auto lastCheck = getLastCheckTimestamp();

        // our build timestamp is newer than the last check
        if (lastCheck <= ECLIPSE_TRANSLATION_TIMESTAMP) {
            // remove local languages (since they are outdated)
            geode::log::info("Removing outdated languages...");
            static auto langsPath = geode::Mod::get()->getSaveDir() / "languages";
            std::error_code ec;
            std::filesystem::remove_all(langsPath, ec);
            if (ec) {
                geode::log::warn("Failed to remove languages directory: {}", ec.message().c_str());
            }
            setLastCheckTimestamp();
            return;
        }

        // perform a check once per day
        auto now = utils::getTimestamp<utils::seconds>();
        constexpr auto day = 24 * 60 * 60;
        if (now - lastCheck < day) return;

        geode::log::info("Checking for language updates...");

        using namespace geode::utils;
        web::WebRequest()
            .get("https://raw.githubusercontent.com/EclipseMenu/translations/refs/heads/metadata/metadata.json")
            .listen([](web::WebResponse* value) {
                if (!value->ok()) {
                    return;
                }

                auto res = value->json();
                if (!res) {
                    geode::log::warn("Failed to parse language metadata");
                    return;
                }

                (void) handleLanguageMetadata(*res);
            });
    }

    size_t getLanguageIndex() {
        auto langs = fetchAvailableLanguages();
        auto it = std::ranges::find_if(langs, [&](auto const& lang) {
            return lang.code == config::get<std::string>("language", DEFAULT_LANGUAGE);
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
