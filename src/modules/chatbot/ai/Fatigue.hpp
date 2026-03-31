#pragma once

#include <cstddef>
#include <cstdint>
#include <deque>

#include "Emotions.hpp"
#include "Intents.hpp"

namespace eclipse::ai {
    enum class FatigueLevel : uint8_t {
        NONE,
        MILD,
        MODERATE,
        HIGH
    };

    struct IntentRecord {
        int count = 0;
        float weight = 0.f;
    };

    FatigueLevel fatigueFromString(std::string_view fatigue);

    class FatigueTracker {
    public:
        explicit FatigueTracker(float decayRate = 0.25f, size_t maxHistory = 6);

        FatigueLevel record(Intent intent, std::string const& raw);
        [[nodiscard]] FatigueLevel currentLevel(Intent intent) const;

        static VAD fatigueVadDelta(FatigueLevel level);

        void decay();
        void reset();

    private:
        static uint64_t messageHash(std::string const& raw);
        static FatigueLevel weightToLevel(float weight);
        [[nodiscard]] bool isNearDuplicate(uint64_t hash) const;

        std::unordered_map<Intent, IntentRecord> m_records;
        std::deque<uint64_t> m_recentHashes;
        size_t m_maxHistory;
        float m_decayRate;
    };
}