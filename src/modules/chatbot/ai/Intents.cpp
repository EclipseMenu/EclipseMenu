#include "Intents.hpp"

#include <algorithm>
#include <bit>
#include <sstream>

#include "Stemmer.hpp"

namespace eclipse::ai {
    PhraseMatcher::PhraseMatcher(size_t windowSize) : m_windowSize(windowSize) {}

    void PhraseMatcher::annotate(PhraseMatch& match, std::span<std::string const> stemmed) const {
        match.negationMult = this->scanNegation(stemmed, match.tokenStart);
        match.modifierMult = this->scanModifier(stemmed, match.tokenStart);
    }

    float PhraseMatcher::scanNegation(std::span<std::string const> stemmed, size_t matchStart) const {
        size_t start = (matchStart >= m_windowSize) ? (matchStart - m_windowSize) : 0;
        for (size_t i = start; i < matchStart; ++i) {
            if (isNegator(stemmed[i])) {
                return -1.f;
            }
        }
        return 1.f;
    }

    float PhraseMatcher::scanModifier(std::span<std::string const> stemmed, size_t matchStart) const {
        float mult = 1.f;
        size_t start = (matchStart >= m_windowSize) ? (matchStart - m_windowSize) : 0;
        for (size_t i = start; i < matchStart; ++i) {
            mult *= modifierValue(stemmed[i]);
        }
        return mult;
    }

    bool PhraseMatcher::isNegator(std::string_view stem) {
        static constexpr std::array negators = std::to_array<std::string_view>(
            {
                "not", "never", "no", "dont", "wont", "cant", "isnt", "arent",
                "without", "hardli", "bare", "neither", "nor", "nope", "noth",
                "denot", "refus", "stop", "lack"
            }
        );

        return std::ranges::any_of(
            negators, [stem](auto const& neg) {
                return neg == stem;
            }
        );
    }

    float PhraseMatcher::modifierValue(std::string_view stem) {
        static constexpr std::array modifiers = std::to_array<std::pair<std::string_view, float>>(
            {
                // intensifiers
                {"veri", 1.6f}, {"realli", 1.6f}, {"extrem", 1.8f},
                {"absolut", 1.7f}, {"complet", 1.7f}, {"total", 1.6f},
                {"incredibl", 1.5f}, {"incredi", 1.5f}, {"so", 1.3f},
                {"such", 1.2f}, {"super", 1.4f}, {"high", 1.3f},
                {"deep", 1.3f}, {"strongli", 1.5f}, {"utter", 1.7f},
                {"massiv", 1.5f}, {"pure", 1.4f},

                // diminishers
                {"kind", 0.5f}, {"sort", 0.5f}, {"somewhat", 0.6f},
                {"slight", 0.5f}, {"almost", 0.7f}, {"littl", 0.6f},
                {"bit", 0.6f}, {"rather", 0.7f}, {"fairli", 0.7f},
                {"mildli", 0.5f}, {"mayb", 0.6f}, {"perhap", 0.6f},
                {"half", 0.5f}, {"partial", 0.6f},
            }
        );

        auto it = std::ranges::find_if(
            modifiers,
            [stem](auto const& pair) { return pair.first == stem; }
        );

        return it != modifiers.end() ? it->second : 1.f;
    }

    IntentEngine::IntentEngine() = default;

    std::vector<IntentResult> IntentEngine::classify(std::string_view input, size_t topK) const {
        auto stems = tokenize(input);

        std::vector<IntentResult> results;
        results.reserve(m_intentKeywords.size());

        float total = 0.0f;
        for (auto const& [intent, entries] : m_intentKeywords) {
            IntentResult r;
            r.intent = intent;
            r.confidence = calculateScore(stems, entries, r.matches);
            total += std::max(0.0f, r.confidence);
            results.push_back(std::move(r));
        }

        if (total > 0.0f) for (auto& r : results) r.confidence /= total;
        else for (auto& r : results) r.confidence = 0.0f;

        std::ranges::sort(
            results, [](IntentResult const& a, IntentResult const& b) {
                return a.confidence > b.confidence;
            }
        );

        if (results[0].confidence == 0.0f)
            results[0].intent = Intent::INVALID;

        results.resize(std::min(topK, results.size()));
        return results;
    }

    std::vector<std::string> IntentEngine::tokenize(std::string_view input) {
        std::vector<std::string> stems;
        std::istringstream ss{std::string(input)};
        std::string token;
        while (ss >> token) {
            std::erase_if(token, [](unsigned char c) { return !std::isalpha(c); });
            if (token.empty()) continue;
            std::ranges::transform(token, token.begin(), ::tolower);
            stems.push_back(PorterStemmer::stem(std::move(token)));
        }
        return stems;
    }

    float IntentEngine::calculateScore(
        std::span<std::string> stems,
        std::span<KeywordEntry const> keywords,
        std::vector<PhraseMatch>& outMatches
    ) const {
        float score = 0.0f;
        for (auto const& keyword : keywords) {
            for (auto& pm : matchPhrase(stems, keyword)) {
                score += pm.effectiveWeight();
                outMatches.push_back(pm);
            }
        }
        return score;
    }

    std::vector<PhraseMatch> IntentEngine::matchPhrase(
        std::span<std::string const> tokens,
        KeywordEntry const& entry
    ) const {
        std::vector<PhraseMatch> matches;
        auto const& phrase = entry.phrase;
        size_t plen = phrase.size();
        if (plen > tokens.size()) return matches;

        for (size_t i = 0; i <= tokens.size() - plen; ++i) {
            bool match = true;
            for (size_t j = 0; j < plen; ++j) {
                if (tokens[i + j] != phrase[j]) {
                    match = false;
                    break;
                }
            }

            if (match) {
                PhraseMatch pm;
                pm.tokenStart = i;
                pm.tokenEnd = i + plen;
                pm.rawWeight = entry.weight;
                m_phraseMatcher.annotate(pm, tokens);
                matches.push_back(pm);
            }
        }

        return matches;
    }

    ComboDetector::ComboDetector() = default;

    std::optional<std::reference_wrapper<ComboEntry const>> ComboDetector::detect(
        std::span<IntentResult const> results,
        float threshold
    ) const {
        std::vector<Intent> active;
        for (auto const& r : results) {
            if (r.intent == Intent::INVALID) continue;
            if (r.confidence < 0.f) continue;
            if (r.confidence >= threshold) {
                active.push_back(r.intent);
            }
        }

        if (active.size() < 2) return std::nullopt;

        std::ranges::sort(active);
        active.erase(
            std::ranges::unique(active).begin(),
            active.end()
        );

        return findBestMatch(active);
    }

    void ComboDetector::registerCombo(ComboEntry entry) {
        std::ranges::sort(entry.key.intents);
        entry.key.intents.erase(
            std::ranges::unique(entry.key.intents).begin(),
            entry.key.intents.end()
        );
        m_entries[entry.key] = std::move(entry);
    }

    std::optional<std::reference_wrapper<ComboEntry const>> ComboDetector::findBestMatch(std::span<Intent const> intents) const {
        auto n = static_cast<uint32_t>(intents.size());
        if (n > 16) return std::nullopt;

        std::vector<uint32_t> masks;
        masks.reserve((1 << n) - n - 1);
        for (uint32_t mask = 1; mask < (1 << n); ++mask) {
            if (std::popcount(mask) < 2) continue;
            masks.push_back(mask);
        }

        std::ranges::stable_sort(masks, [](uint32_t a, uint32_t b) {
            return std::popcount(a) > std::popcount(b);
        });

        for (uint32_t mask : masks) {
            ComboKey key;
            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) {
                    key.intents.push_back(intents[i]);
                }
            }
            auto it = m_entries.find(key);
            if (it != m_entries.end()) {
                return std::ref(it->second);
            }
        }

        return std::nullopt;
    }
}