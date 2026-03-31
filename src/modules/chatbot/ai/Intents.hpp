#pragma once

#include <span>
#include <string>
#include <unordered_map>
#include <vector>

#include "Handles.hpp"

namespace eclipse::ai {
    struct PhraseMatch {
        size_t tokenStart;
        size_t tokenEnd;
        float rawWeight;

        float negationMult = 1.f;
        float modifierMult = 1.f;

        float effectiveWeight() const {
            return rawWeight * modifierMult * negationMult;
        }
    };

    class PhraseMatcher {
    public:
        explicit PhraseMatcher(size_t windowSize = 3);
        void annotate(PhraseMatch& match, std::span<std::string const> stemmed) const;

    private:
        float scanNegation(std::span<std::string const> stemmed, size_t matchStart) const;
        float scanModifier(std::span<std::string const> stemmed, size_t matchStart) const;

        static bool isNegator(std::string_view stem);
        static float modifierValue(std::string_view stem);

    private:
        size_t m_windowSize;
    };

    struct IntentResult {
        Intent intent;
        float confidence;
        std::vector<PhraseMatch> matches;
    };

    struct KeywordEntry {
        std::vector<std::string> phrase;
        float weight;

        static KeywordEntry unigram(std::string phrase, float weight) {
            return { { std::move(phrase) }, weight };
        }

        static KeywordEntry phraseOf(std::vector<std::string> stems, float weight) {
            return { std::move(stems), weight };
        }
    };

    class IntentEngine {
    public:
        IntentEngine();

        [[nodiscard]] std::vector<IntentResult> classify(std::string_view input, size_t topK = 5) const;
        void clear() { m_intentKeywords.clear(); }

        void addIntent(Intent intent, std::vector<KeywordEntry> keywords) {
            m_intentKeywords[intent] = std::move(keywords);
        }
        void addKeyword(Intent intent, KeywordEntry keyword) {
            m_intentKeywords[intent].push_back(std::move(keyword));
        }

    private:
        static std::vector<std::string> tokenize(std::string_view input);
        [[nodiscard]] float calculateScore(std::span<std::string> stems, std::span<KeywordEntry const> keywords, std::vector<PhraseMatch>& outMatches) const;
        [[nodiscard]] std::vector<PhraseMatch> matchPhrase(std::span<std::string const> tokens, KeywordEntry const& entry) const;

    private:
        std::unordered_map<Intent, std::vector<KeywordEntry>> m_intentKeywords;
        PhraseMatcher m_phraseMatcher;
    };

    struct ComboKey {
        std::vector<Intent> intents;
        bool operator==(ComboKey const& other) const {
            return intents == other.intents;
        }
    };

    struct ComboKeyHash {
        size_t operator()(ComboKey const& key) const {
            size_t hash = 0;
            for (auto const& intent : key.intents) {
                hash ^= std::hash<Intent>{}(intent) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
            }
            return hash;
        }
    };

    struct ComboEntry {
        ComboKey key;
        std::string templateTag;
        std::optional<Intent> vadOverride;
    };

    class ComboDetector {
    public:
        ComboDetector();

        [[nodiscard]] std::optional<std::reference_wrapper<ComboEntry const>> detect(
            std::span<IntentResult const> results,
            float threshold = 0.15
        ) const;

        void registerCombo(ComboEntry entry);
        void clear() { m_entries.clear(); }

    private:
        [[nodiscard]] std::optional<std::reference_wrapper<ComboEntry const>> findBestMatch(std::span<Intent const> intents) const;

    private:
        std::unordered_map<ComboKey, ComboEntry, ComboKeyHash> m_entries;
    };
}

