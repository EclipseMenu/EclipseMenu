#include "Context.hpp"

namespace eclipse::ai {
    void Context::addEntry(ContextEntry entry) {
        if (m_entries.size() >= m_maxEntries) {
            m_entries.pop_front();
        }
        m_entries.push_back(std::move(entry));
    }

    std::optional<std::reference_wrapper<ContextEntry const>> Context::last() const {
        if (m_entries.empty()) {
            return std::nullopt;
        }
        return m_entries.back();
    }

    std::optional<Intent> Context::lastIntent() const {
        if (m_entries.empty()) {
            return std::nullopt;
        }
        return m_entries.back().intent;
    }

    std::optional<Entity> Context::findRecentEntity(EntityType type, size_t lookback) const {
        size_t count = 0;
        for (auto it = m_entries.rbegin(); it != m_entries.rend() && count < lookback; ++it, ++count) {
            for (auto const& entity : it->entities) {
                if (entity.type == type) {
                    return entity;
                }
            }
        }
        return std::nullopt;
    }

    bool Context::containsIntent(Intent intent, size_t lookback) const {
        size_t count = 0;
        for (auto it = m_entries.rbegin(); it != m_entries.rend() && count < lookback; ++it, ++count) {
            if (it->intent == intent) {
                return true;
            }
        }
        return false;
    }
}