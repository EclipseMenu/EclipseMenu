#include "Emotions.hpp"

namespace eclipse::ai {
    EmotionModel::EmotionModel() = default;

    void EmotionModel::applyIntent(Intent intent, float confidence) {
        auto it = m_intentVADs.find(intent);
        if (it != m_intentVADs.end()) {
            m_state = (m_state + it->second * confidence).clamp();
        }
    }

    void EmotionModel::decay(float rate) {
        m_state.valence += (m_baseline.valence - m_state.valence) * rate;
        m_state.arousal += (m_baseline.arousal - m_state.arousal) * rate;
        m_state.dominance += (m_baseline.dominance - m_state.dominance) * rate;
    }

    Emotion EmotionModel::currentEmotion() const {
        auto closest = Emotion::INVALID;
        float closestDist = std::numeric_limits<float>::max();

        for (auto const& [emotion, centroid] : m_centroids) {
            float dist = m_state.distanceSq(centroid);
            if (dist < closestDist) {
                closestDist = dist;
                closest = emotion;
            }
        }

        return closest;
    }
}