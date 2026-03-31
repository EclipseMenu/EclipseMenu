#pragma once

#include <filesystem>
#include <string>
#include <Geode/Result.hpp>

#include "Context.hpp"
#include "Emotions.hpp"
#include "Entities.hpp"
#include "Fatigue.hpp"
#include "Intents.hpp"
#include "Slots.hpp"
#include "Templates.hpp"

namespace eclipse::ai {
    class Chatbot {
    public:
        Chatbot();

        geode::Result<> loadConfig(std::filesystem::path const& path);
        std::string process(std::string const& input);

        void setupActions();

    private:
        [[nodiscard]] Intent resolveIntent(Intent intent, std::span<Entity const> entities) const;

    private:
        IntentRegistry m_intentRegistry;
        IntentEngine m_intentEngine;

        EmotionRegistry m_emotionRegistry;
        EmotionModel m_emotionModel;

        Context m_context;
        EntityExtractor m_entityExtractor;
        ActionRegistry m_actionRegistry;
        FatigueTracker m_fatigueTracker;
        TemplateEngine m_templateEngine;
        ComboDetector m_comboDetector;

        SlotRegistry m_slotRegistry;
        SlotFiller m_slotFiller;

        geode::utils::StringMap<std::string> m_settingNameMap;
        geode::utils::StringMap<std::string> m_settingActualNames;

        float m_emotionDecay = 0.05f;
        Intent m_mathQueryIntent = Intent::INVALID;
        Intent m_enableSettingIntent = Intent::INVALID;
        Intent m_disableSettingIntent = Intent::INVALID;
        Intent m_checkSettingStateIntent = Intent::INVALID;
        Intent m_complaintIntent = Intent::INVALID;
    };
}