#pragma once

#include <algorithm>
#include <matjson.hpp>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Handles.hpp"

namespace eclipse::ai {
    struct VAD {
        float valence = 0.f;
        float arousal = 0.f;
        float dominance = 0.f;

        constexpr VAD operator+(VAD const& other) const {
            return { valence + other.valence, arousal + other.arousal, dominance + other.dominance };
        }

        constexpr VAD& operator+=(VAD const& other) {
            valence += other.valence;
            arousal += other.arousal;
            dominance += other.dominance;
            return *this;
        }

        constexpr VAD operator*(float scalar) const {
            return { valence * scalar, arousal * scalar, dominance * scalar };
        }

        constexpr float distanceSq(VAD const& other) const {
            float dv = valence - other.valence;
            float da = arousal - other.arousal;
            float dd = dominance - other.dominance;
            return dv * dv + da * da + dd * dd;
        }

        constexpr VAD clamp() const {
            return {
                std::clamp(valence, -1.f, 1.f),
                std::clamp(arousal, -1.f, 1.f),
                std::clamp(dominance, -1.f, 1.f)
            };
        }
    };

    class EmotionModel {
    public:
        EmotionModel();

        void applyIntent(Intent intent, float confidence);

        void decay(float rate = 0.05f);
        void nudge(VAD const& delta) { m_state = (m_state + delta).clamp(); }

        Emotion currentEmotion() const;
        VAD const& currentVAD() const { return m_state; }

        void setBaseline(VAD baseline) { m_baseline = baseline; }
        void addEmotionCentroid(Emotion state, VAD vad) { m_centroids.push_back({state, vad}); }
        void setIntentVAD(Intent intent, VAD vad) { m_intentVADs[intent] = std::move(vad); }
        void clear() {
            m_intentVADs.clear();
            m_centroids.clear();
        }

    private:
        struct EmotionCentroid {
            Emotion state;
            VAD vad;
        };

        std::unordered_map<Intent, VAD> m_intentVADs;
        std::vector<EmotionCentroid> m_centroids;

        VAD m_state;
        VAD m_baseline = {0.3, 0.1, 0.2};
    };
}

template <>
struct matjson::Serialize<eclipse::ai::VAD> {
    static geode::Result<eclipse::ai::VAD> fromJson(Value const& value) {
        if (!value.isArray() || value.size() != 3) {
            return geode::Err("Expected array of 3 elements for VAD");
        }

        GEODE_UNWRAP_INTO(double valence, value[0].as<double>());
        GEODE_UNWRAP_INTO(double arousal, value[1].as<double>());
        GEODE_UNWRAP_INTO(double dominance, value[2].as<double>());

        return geode::Ok(eclipse::ai::VAD{
            static_cast<float>(valence),
            static_cast<float>(arousal),
            static_cast<float>(dominance)
        });
    }

    static Value toJson(eclipse::ai::VAD const& value) {
        auto arr = Value::array();
        arr.push(value.valence);
        arr.push(value.arousal);
        arr.push(value.dominance);
        return arr;
    }
};