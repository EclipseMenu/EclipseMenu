#include "Templates.hpp"

namespace eclipse::ai {
    static bool isSlotChar(char c) {
        return std::isalnum(static_cast<uint8_t>(c)) || c == '_';
    }

    static bool tryParseSlotToken(std::string_view text, size_t start, std::string_view& slotName, size_t& tokenLen) {
        if (start + 4 > text.size() || text[start] != '{' || text[start + 1] != '{') {
            return false;
        }

        size_t nameStart = start + 2;
        size_t pos = nameStart;
        while (pos < text.size() && isSlotChar(text[pos])) {
            ++pos;
        }
        if (pos == nameStart) {
            return false;
        }
        if (pos + 1 >= text.size() || text[pos] != '}' || text[pos + 1] != '}') {
            return false;
        }

        slotName = text.substr(nameStart, pos - nameStart);
        tokenLen = (pos + 2) - start;
        return true;
    }

    void SlotPool::add(Emotion emotion, FatigueLevel fatigue, std::vector<std::string> words) {
        m_pool[{emotion, fatigue}] = std::move(words);
    }

    void SlotPool::add(Emotion emotion, std::vector<std::string> words) {
        m_pool[{emotion, FatigueLevel::NONE}] = words;
        m_pool[{emotion, FatigueLevel::MILD}] = words;
        m_pool[{emotion, FatigueLevel::MODERATE}] = words;
        m_pool[{emotion, FatigueLevel::HIGH}] = std::move(words);
    }

    void SlotPool::add(FatigueLevel fatigue, std::vector<std::string> words) {
        m_pool[{Emotion::INVALID, fatigue}] = std::move(words);
    }

    void SlotPool::add(std::vector<std::string> words) {
        m_pool[{Emotion::INVALID, static_cast<FatigueLevel>(0xFF)}] = std::move(words);
    }

    std::vector<std::string> const* SlotPool::resolve(Emotion e, FatigueLevel f) const {
        // exact match
        auto it = m_pool.find({e, f});
        if (it != m_pool.end()) return &it->second;

        // emotion match
        it = m_pool.find({e, FatigueLevel::NONE});
        if (it != m_pool.end()) return &it->second;

        // fatigue match
        it = m_pool.find({Emotion::INVALID, f});
        if (it != m_pool.end()) return &it->second;

        // default fallback
        it = m_pool.find({Emotion::INVALID, static_cast<FatigueLevel>(0xFF)});
        if (it != m_pool.end()) return &it->second;

        return nullptr;
    }

    TemplateEngine::TemplateEngine() = default;

    std::string TemplateEngine::generate(Intent intent, Emotion emotion, FatigueLevel fatigue) const {
        auto tmpls = resolveTemplate(intent, emotion, fatigue);
        if (tmpls && !tmpls->empty()) {
            return sanitizeExpanded(expand(pick(*tmpls), emotion, fatigue));
        }
        return sanitizeExpanded(expand("{{fallback}}", emotion, fatigue));
    }

    std::string TemplateEngine::generateCombo(
        std::string const& tag, Emotion emotion, FatigueLevel fatigue
    ) const {
        auto tagIt = m_comboTemplates.find(tag);
        if (tagIt != m_comboTemplates.end()) {
            auto fatigueIt = tagIt->second.find(fatigue);
            if (fatigueIt == tagIt->second.end()) {
                fatigueIt = tagIt->second.find(FatigueLevel::NONE);
            }
            if (fatigueIt != tagIt->second.end() && !fatigueIt->second.empty()) {
                return sanitizeExpanded(expand(pick(fatigueIt->second), emotion, fatigue));
            }
        }
        return sanitizeExpanded(expand("{{fallback}}", emotion, fatigue));
    }

    void TemplateEngine::addSlot(std::string name, SlotPool pool) {
        m_slots[std::move(name)] = std::move(pool);
    }

    void TemplateEngine::addTemplate(
        Intent intent,
        Emotion emotion,
        FatigueLevel fatigue,
        std::vector<std::string> templates
    ) {
        m_templates[intent][{emotion, fatigue}] = std::move(templates);
    }

    void TemplateEngine::addTemplate(Intent intent, Emotion emotion, std::vector<std::string> templates) {
        m_templates[intent][{emotion, FatigueLevel::NONE}] = templates;
        m_templates[intent][{emotion, FatigueLevel::MILD}] = templates;
        m_templates[intent][{emotion, FatigueLevel::MODERATE}] = templates;
        m_templates[intent][{emotion, FatigueLevel::HIGH}] = std::move(templates);
    }

    void TemplateEngine::addComboTemplate(
        std::string tag,
        FatigueLevel fatigue,
        std::vector<std::string> templates
    ) {
        m_comboTemplates[std::move(tag)][fatigue] = std::move(templates);
    }

    std::vector<std::string> const* TemplateEngine::resolveTemplate(
        Intent intent,
        Emotion emotion,
        FatigueLevel fatigue
    ) const {
        auto it = m_templates.find(intent);
        if (it == m_templates.end()) return nullptr;

        auto ki = it->second.find({emotion, fatigue});
        if (ki != it->second.end()) return &ki->second;

        ki = it->second.find({emotion, FatigueLevel::NONE});
        if (ki != it->second.end()) return &ki->second;

        ki = it->second.find({Emotion::INVALID, fatigue});
        if (ki != it->second.end()) return &ki->second;

        ki = it->second.find({Emotion::INVALID, FatigueLevel::NONE});
        if (ki != it->second.end()) return &ki->second;

        if (!it->second.empty()) return &it->second.begin()->second;

        return nullptr;
    }

    std::string TemplateEngine::expand(
        std::string const& tmpl,
        Emotion emotion,
        FatigueLevel fatigue,
        int depth
    ) const {
        if (depth > 10) return tmpl; // prevent infinite recursion

        std::string result;
        result.reserve(tmpl.size() + 20);

        std::string_view text(tmpl);
        size_t i = 0;
        while (i < text.size()) {
            std::string_view slotName;
            size_t tokenLen = 0;
            if (tryParseSlotToken(text, i, slotName, tokenLen)) {
                auto poolIt = m_slots.find(std::string(slotName));
                if (poolIt == m_slots.end()) {
                    result.append("{{").append(slotName).append("}}");
                    i += tokenLen;
                    continue;
                }

                auto const* candidates = poolIt->second.resolve(emotion, fatigue);
                if (candidates && !candidates->empty()) {
                    result.append(expand(pick(*candidates), emotion, fatigue, depth + 1));
                }

                i += tokenLen;
                continue;
            }

            result.push_back(text[i]);
            ++i;
        }
        return result;
    }

    std::string const& TemplateEngine::pick(std::span<std::string const> options) const {
        if (options.size() == 1) {
            return options[0];
        }

        using geode::utils::random::generate;

        size_t key = reinterpret_cast<size_t>(options.data()) ^ (options.size() << 1);
        size_t index = generate(0, options.size());
        if (auto it = m_lastPickByPool.find(key); it != m_lastPickByPool.end()) {
            size_t attempts = 4;
            while (attempts-- > 0 && index == it->second) {
                index = generate(0, options.size());
            }
        }

        m_lastPickByPool[key] = index;
        return options[index];
    }

    std::string TemplateEngine::sanitizeExpanded(std::string const& text) {
        std::string stripped;
        stripped.reserve(text.size());

        std::string_view view(text);
        for (size_t i = 0; i < view.size();) {
            std::string_view slotName;
            size_t tokenLen = 0;
            if (tryParseSlotToken(view, i, slotName, tokenLen)) {
                i += tokenLen;
                continue;
            }

            stripped.push_back(view[i]);
            ++i;
        }

        std::string compact;
        compact.reserve(stripped.size());

        bool prevSpace = true;
        for (char c : stripped) {
            if (std::isspace(static_cast<uint8_t>(c))) {
                if (!prevSpace) {
                    compact.push_back(' ');
                }
                prevSpace = true;
            } else {
                compact.push_back(c);
                prevSpace = false;
            }
        }

        if (!compact.empty() && compact.back() == ' ') {
            compact.pop_back();
        }

        return compact;
    }
}