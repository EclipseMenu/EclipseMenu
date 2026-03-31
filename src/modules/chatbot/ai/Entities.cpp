#include "Entities.hpp"

#include <Geode/utils/general.hpp>

#include <algorithm>
#include <cctype>
#include <numeric>
#include <sstream>
#include <unordered_set>

namespace eclipse::ai {
    static bool isWordChar(char c) {
        return std::isalnum(static_cast<uint8_t>(c)) || c == '_';
    }

    static size_t skipSpaces(std::string const& input, size_t pos) {
        while (pos < input.size() && std::isspace(static_cast<uint8_t>(input[pos]))) {
            ++pos;
        }
        return pos;
    }

    static bool parseNumberToken(std::string const& input, size_t start, size_t& end) {
        size_t pos = start;
        if (pos >= input.size() || !std::isdigit(static_cast<uint8_t>(input[pos]))) {
            return false;
        }

        while (pos < input.size() && std::isdigit(static_cast<uint8_t>(input[pos]))) {
            ++pos;
        }

        if (pos < input.size() && input[pos] == '.') {
            size_t fracStart = pos + 1;
            size_t fracEnd = fracStart;
            while (fracEnd < input.size() && std::isdigit(static_cast<uint8_t>(input[fracEnd]))) {
                ++fracEnd;
            }
            if (fracEnd > fracStart) {
                pos = fracEnd;
            }
        }

        end = pos;
        return true;
    }

    static std::string_view numberPrefix(std::string_view input) {
        if (input.empty() || !std::isdigit(static_cast<uint8_t>(input.front()))) {
            return {};
        }

        size_t end = 0;
        while (end < input.size() && std::isdigit(static_cast<uint8_t>(input[end]))) {
            ++end;
        }

        // include decimal part
        if (end < input.size() && input[end] == '.') {
            size_t fracStart = end + 1;
            size_t fracEnd = fracStart;
            while (fracEnd < input.size() && std::isdigit(static_cast<uint8_t>(input[fracEnd]))) {
                ++fracEnd;
            }
            if (fracEnd > fracStart) {
                end = fracEnd;
            }
        }

        return input.substr(0, end);
    }

    static bool parseBinaryMath(std::string const& input, size_t start, size_t& end) {
        size_t pos = start;
        if (!parseNumberToken(input, pos, pos)) {
            return false;
        }

        size_t afterFirst = skipSpaces(input, pos);
        if (afterFirst >= input.size()) {
            return false;
        }

        auto isOp = [](char c) {
            return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
        };

        if (!isOp(input[afterFirst])) {
            return false;
        }

        pos = skipSpaces(input, afterFirst + 1);
        if (!parseNumberToken(input, pos, pos)) {
            return false;
        }

        while (true) {
            size_t maybeOp = skipSpaces(input, pos);
            if (maybeOp >= input.size() || !isOp(input[maybeOp])) {
                end = pos;
                return true;
            }

            size_t next = skipSpaces(input, maybeOp + 1);
            if (!parseNumberToken(input, next, pos)) {
                end = pos;
                return true;
            }
        }
    }

    static bool parseFunctionMath(std::string const& input, size_t start, size_t& end) {
        static constexpr std::string_view kFunctions[] = {
            "sqrt", "log", "abs", "sin", "cos", "tan"
        };

        size_t pos = start;
        bool matched = false;
        for (auto fn : kFunctions) {
            if (input.compare(start, fn.size(), fn) == 0) {
                pos = start + fn.size();
                matched = true;
                break;
            }
        }
        if (!matched || pos >= input.size()) {
            return false;
        }

        if (input[pos] != '(' && input[pos] != ' ') {
            return false;
        }
        pos = skipSpaces(input, pos + 1);

        if (!parseNumberToken(input, pos, pos)) {
            return false;
        }

        pos = skipSpaces(input, pos);
        if (pos < input.size() && (input[pos] == ')' || input[pos] == ' ')) {
            ++pos;
        }

        end = pos;
        return true;
    }

    EntityExtractor::EntityExtractor(std::vector<std::string> settings)
        : m_settings(std::move(settings)) {}

    std::vector<Entity> EntityExtractor::extract(std::string const& input) const {
        std::vector<Entity> entities = extractQuoted(input);
        auto math = extractMath(input);
        auto numbers = extractNumbers(input);
        auto settings = extractSettings(input);

        entities.reserve(entities.size() + math.size() + numbers.size() + settings.size());
        entities.insert(entities.end(), std::make_move_iterator(math.begin()), std::make_move_iterator(math.end()));
        entities.insert(entities.end(), std::make_move_iterator(numbers.begin()), std::make_move_iterator(numbers.end()));
        entities.insert(entities.end(), std::make_move_iterator(settings.begin()), std::make_move_iterator(settings.end()));

        return entities;
    }

    void EntityExtractor::setSettings(std::vector<std::string> settings) {
        m_settings = std::move(settings);
    }

    size_t EntityExtractor::levenshtein(std::string_view s1, std::string_view s2) {
        if (s1.size() < s2.size()) {
            std::swap(s1, s2);
        }

        std::vector<size_t> prev(s2.size() + 1), curr(s2.size() + 1);
        std::iota(prev.begin(), prev.end(), 0);

        for (size_t i = 1; i <= s1.size(); ++i) {
            curr[0] = i;
            for (size_t j = 1; j <= s2.size(); ++j) {
                size_t cost = (std::tolower(s1[i - 1]) == std::tolower(s2[j - 1])) ? 0 : 1;
                curr[j] = std::min({ prev[j] + 1, curr[j - 1] + 1, prev[j - 1] + cost });
            }
            std::swap(prev, curr);
        }

        return prev[s2.size()];
    }

    float EntityExtractor::fuzzyScore(std::string_view haystack, std::string_view needle) {
        size_t maxLen = std::max(haystack.size(), needle.size());
        if (maxLen == 0) return 1.0f;
        size_t dist = levenshtein(haystack, needle);
        return 1.0f - static_cast<float>(dist) / maxLen;
    }

    std::vector<Entity> EntityExtractor::extractQuoted(std::string const& input) {
        std::vector<Entity> out;
        size_t i = 0;
        while (i < input.size()) {
            char quote = input[i];
            if (quote != '\'' && quote != '"') {
                ++i;
                continue;
            }

            size_t end = input.find(quote, i + 1);
            if (end == std::string::npos || end == i + 1) {
                ++i;
                continue;
            }

            auto raw = input.substr(i, end - i + 1);
            auto val = raw.substr(1, raw.size() - 2);
            out.push_back({ std::move(raw), std::move(val), 1.0f, EntityType::QUOTED_STRING });
            i = end + 1;
        }
        return out;
    }

    std::vector<Entity> EntityExtractor::extractMath(std::string const& input) {
        std::vector<Entity> out;
        size_t i = 0;
        while (i < input.size()) {
            size_t end = i;
            if (parseBinaryMath(input, i, end) || parseFunctionMath(input, i, end)) {
                out.push_back({ input.substr(i, end - i), {}, 1.0f, EntityType::MATH_EXPRESSION });
                i = end;
                continue;
            }
            ++i;
        }
        return out;
    }

    std::vector<Entity> EntityExtractor::extractNumbers(std::string const& input) {
        std::vector<Entity> out;
        size_t i = 0;
        while (i < input.size()) {
            if (!std::isdigit(static_cast<uint8_t>(input[i]))) {
                ++i;
                continue;
            }

            if (i > 0 && isWordChar(input[i - 1])) {
                ++i;
                continue;
            }

            size_t end = i;
            if (!parseNumberToken(input, i, end)) {
                ++i;
                continue;
            }

            if (end < input.size() && isWordChar(input[end])) {
                i = end;
                continue;
            }

            auto raw = input.substr(i, end - i);
            auto numericView = numberPrefix(raw);
            auto parsed = geode::utils::numFromString<double>(numericView);
            if (!parsed) {
                i = end;
                continue;
            }
            out.push_back({ std::move(raw), parsed.unwrap(), 1.0f, EntityType::NUMBER });
            i = end;
        }
        return out;
    }

    std::vector<Entity> EntityExtractor::extractSettings(std::string const& input) const {
        if (m_settings.empty()) return {};

        std::vector<std::string> tokens;
        {
            std::istringstream iss(input);
            std::string token;
            while (iss >> token) {
                tokens.push_back(std::move(token));
            }
        }

        std::vector<Entity> out;
        constexpr size_t MAX_WINDOW = 4;

        for (size_t start = 0; start < tokens.size(); ++start) {
            for (size_t len = 1; len <= std::min(MAX_WINDOW, tokens.size() - start); ++len) {
                std::string window;
                for (size_t k = start; k < start + len; ++k) {
                    if (k > start) window.push_back(' ');
                    window.append(tokens[k]);
                }

                for (auto const& setting : m_settings) {
                    float score = fuzzyScore(window, setting);
                    if (score >= FUZZY_THRESHOLD) {
                        out.push_back({ std::move(window), setting, score, EntityType::SETTING_NAME });
                    }
                }
            }
        }

        std::ranges::sort(out, [](Entity const& a, Entity const& b) {
            return a.confidence > b.confidence;
        });

        std::unordered_set<std::string> seen;
        std::vector<Entity> uniqueOut;
        for (auto const& entity : out) {
            auto& name = std::get<std::string>(entity.value);
            if (seen.insert(name).second) {
                uniqueOut.push_back(std::move(entity));
            }
        }

        return uniqueOut;
    }

    ActionRegistry::ActionRegistry() = default;

    void ActionRegistry::registerAction(Intent intent, ActionHandler handler) {
        m_actions.insert_or_assign(intent, std::move(handler));
    }

    std::optional<ActionResult> ActionRegistry::dispatch(Intent intent, EntityMap const& slots) {
        auto it = m_actions.find(intent);
        if (it != m_actions.end()) {
            return it->second(slots);
        }
        return std::nullopt;
    }

    bool ActionRegistry::hasAction(Intent intent) const {
        return m_actions.contains(intent);
    }

    void ActionRegistry::clear() {
        m_actions.clear();
    }
}