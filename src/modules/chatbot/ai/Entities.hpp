#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "Intents.hpp"

namespace eclipse::ai {
    enum class EntityType : uint8_t {
        NUMBER,
        MATH_EXPRESSION,
        SETTING_NAME,
        QUOTED_STRING,
        GENERIC_TERM
    };

    using EntityValue = std::variant<double, std::string>;

    struct Entity {
        std::string raw;
        EntityValue value;
        float confidence;
        EntityType type;
    };

    using EntityMap = std::unordered_map<std::string, Entity>;

    class EntityExtractor {
    public:
        explicit EntityExtractor(std::vector<std::string> settings = {});

        [[nodiscard]] std::vector<Entity> extract(std::string const& input) const;

        void setSettings(std::vector<std::string> settings);
        void addSetting(std::string setting) {
            m_settings.push_back(std::move(setting));
        }

    private:
        static constexpr float FUZZY_THRESHOLD = 0.72f;

        static size_t levenshtein(std::string_view s1, std::string_view s2);
        static float fuzzyScore(std::string_view haystack, std::string_view needle);

        static std::vector<Entity> extractQuoted(std::string const& input);
        static std::vector<Entity> extractMath(std::string const& input);
        static std::vector<Entity> extractNumbers(std::string const& input);
        [[nodiscard]] std::vector<Entity> extractSettings(std::string const& input) const;

        std::vector<std::string> m_settings;
    };

    struct ActionResult {
        std::string response;
        bool success = true;
    };

    using ActionHandler = geode::Function<ActionResult(EntityMap const&)>;

    class ActionRegistry {
    public:
        ActionRegistry();

        void registerAction(Intent intent, ActionHandler handler);
        std::optional<ActionResult> dispatch(Intent intent, EntityMap const& slots);
        [[nodiscard]] bool hasAction(Intent intent) const;
        void clear();

    private:
        std::unordered_map<Intent, ActionHandler> m_actions;
    };
}
