#pragma once

#include <deque>
#include <optional>
#include <string>
#include <vector>

#include "Entities.hpp"
#include "Intents.hpp"

namespace eclipse::ai {
    struct ContextEntry {
        std::string rawText;
        std::vector<Entity> entities;
        Intent intent;
    };

    class Context {
    public:
        explicit Context(size_t maxEntries = 10) : m_maxEntries(maxEntries) {}

        void addEntry(ContextEntry entry);

        [[nodiscard]] std::optional<std::reference_wrapper<ContextEntry const>> last() const;
        [[nodiscard]] std::optional<Intent> lastIntent() const;

        [[nodiscard]] std::optional<Entity> findRecentEntity(EntityType type, size_t lookback = 3) const;

        [[nodiscard]] bool containsIntent(Intent intent, size_t lookback = 3) const;

        [[nodiscard]] size_t size() const { return m_entries.size(); }
        [[nodiscard]] bool empty() const { return m_entries.empty(); }
        void clear() { m_entries.clear(); }

        [[nodiscard]] auto begin() const { return m_entries.begin(); }
        [[nodiscard]] auto end() const { return m_entries.end(); }

    private:
        std::deque<ContextEntry> m_entries;
        size_t m_maxEntries;
    };
}