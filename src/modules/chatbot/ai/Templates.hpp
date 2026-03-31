#pragma once

#include "Emotions.hpp"
#include "Fatigue.hpp"

namespace eclipse::ai {
class SlotPool {
    public:
        void add(Emotion emotion, FatigueLevel fatigue, std::vector<std::string> words);
        void add(Emotion emotion, std::vector<std::string> words);
        void add(FatigueLevel fatigue, std::vector<std::string> words);
        void add(std::vector<std::string> words);

        std::vector<std::string> const* resolve(Emotion e, FatigueLevel f) const;

        struct Key {
            Emotion emotion;
            FatigueLevel fatigue;

            bool operator==(Key const& other) const {
                return emotion == other.emotion && fatigue == other.fatigue;
            }
        };

        struct KeyHash {
            size_t operator()(Key const& k) const {
                return static_cast<size_t>(k.fatigue) << 8 | static_cast<size_t>(k.emotion);
            }
        };

    private:
        std::unordered_map<Key, std::vector<std::string>, KeyHash> m_pool;
    };

    class TemplateEngine {
    public:
        explicit TemplateEngine();

        std::string generate(Intent intent, Emotion emotion, FatigueLevel fatigue) const;
        std::string generateCombo(std::string const& tag, Emotion emotion, FatigueLevel fatigue) const;

        void addSlot(std::string name, SlotPool pool);
        void addTemplate(Intent intent, Emotion emotion, FatigueLevel fatigue, std::vector<std::string> templates);
        void addTemplate(Intent intent, Emotion emotion, std::vector<std::string> templates);
        void addComboTemplate(std::string tag, FatigueLevel fatigue, std::vector<std::string> templates);

        void clear() {
            m_slots.clear();
            m_templates.clear();
            m_comboTemplates.clear();
            m_lastPickByPool.clear();
        }

    private:
        std::vector<std::string> const* resolveTemplate(Intent intent, Emotion emotion, FatigueLevel fatigue) const;
        static std::string sanitizeExpanded(std::string const& text);
        std::string expand(std::string const& tmpl, Emotion emotion, FatigueLevel fatigue, int depth = 0) const;
        std::string const& pick(std::span<std::string const> options) const;

        geode::utils::StringMap<SlotPool> m_slots;
        std::unordered_map<Intent, std::unordered_map<SlotPool::Key, std::vector<std::string>, SlotPool::KeyHash>> m_templates;
        geode::utils::StringMap<std::unordered_map<FatigueLevel, std::vector<std::string>>> m_comboTemplates;
        mutable std::unordered_map<size_t, size_t> m_lastPickByPool;
    };
}