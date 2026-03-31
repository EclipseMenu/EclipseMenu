#include "Fatigue.hpp"

#include <ranges>
#include <sstream>

namespace eclipse::ai {
    FatigueLevel fatigueFromString(std::string_view fatigue) {
        if (fatigue == "NONE") return FatigueLevel::NONE;
        if (fatigue == "MILD") return FatigueLevel::MILD;
        if (fatigue == "MODERATE") return FatigueLevel::MODERATE;
        if (fatigue == "HIGH") return FatigueLevel::HIGH;
        return FatigueLevel::NONE;
    }

    FatigueTracker::FatigueTracker(float decayRate, size_t maxHistory)
        : m_maxHistory(maxHistory), m_decayRate(decayRate) {}

    FatigueLevel FatigueTracker::record(Intent intent, std::string const& raw) {
        auto hash = messageHash(raw);

        float hitWeight = isNearDuplicate(hash) ? 1.5f : 1.f;

        if (m_recentHashes.size() >= m_maxHistory) m_recentHashes.pop_front();
        m_recentHashes.push_back(hash);

        auto& rec = m_records[intent];
        rec.count++;
        rec.weight += hitWeight;

        return weightToLevel(rec.weight);
    }

    FatigueLevel FatigueTracker::currentLevel(Intent intent) const {
        auto it = m_records.find(intent);
        if (it == m_records.end()) return FatigueLevel::NONE;
        return weightToLevel(it->second.weight);
    }

    VAD FatigueTracker::fatigueVadDelta(FatigueLevel level) {
        switch (level) {
            case FatigueLevel::NONE: return {  0.00f,  0.00f,  0.00f };
            case FatigueLevel::MILD: return { -0.05f, -0.05f,  0.00f };
            case FatigueLevel::MODERATE: return { -0.15f, -0.10f,  0.10f };
            case FatigueLevel::HIGH: return { -0.30f,  0.20f,  0.35f };
            default: return {  0.00f,  0.00f,  0.00f };
        }
    }

    void FatigueTracker::decay() {
        for (auto& [count, weight] : m_records | std::views::values) {
            weight *= 1.f - m_decayRate;
        }
    }

    void FatigueTracker::reset() {
        m_records.clear();
        m_recentHashes.clear();
    }

    uint64_t FatigueTracker::messageHash(std::string const& raw) {
        std::vector<std::string> tokens;
        std::istringstream ss(raw);
        std::string tok;
        while (ss >> tok) {
            std::erase_if(tok, [](unsigned char c){ return !std::isalpha(c); });
            if (!tok.empty()) {
                std::ranges::transform(tok, tok.begin(), ::tolower);
                tokens.push_back(std::move(tok));
            }
        }
        std::ranges::sort(tokens);

        constexpr uint64_t FNV_BASIS = 14695981039346656037ULL;
        constexpr uint64_t FNV_PRIME = 1099511628211ULL;

        uint64_t hash = FNV_BASIS;
        for (auto const& t : tokens) {
            for (unsigned char c : t) {
                hash ^= c;
                hash *= FNV_PRIME;
            }
            hash ^= 0x20;
            hash *= FNV_PRIME;
        }

        return hash;
    }

    FatigueLevel FatigueTracker::weightToLevel(float weight) {
        if (weight < 1.5f) return FatigueLevel::NONE;
        if (weight < 2.8f) return FatigueLevel::MILD;
        if (weight < 4.5f) return FatigueLevel::MODERATE;
        return FatigueLevel::HIGH;
    }

    bool FatigueTracker::isNearDuplicate(uint64_t hash) const {
        return std::ranges::find(m_recentHashes, hash) != m_recentHashes.end();
    }
}
