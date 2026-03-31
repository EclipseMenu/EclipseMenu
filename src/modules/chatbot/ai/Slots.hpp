#pragma once

#include <string>
#include <vector>

#include "Entities.hpp"
#include "Intents.hpp"

namespace eclipse::ai {
    struct SlotDefinition {
        std::string name;
        std::vector<std::string> prompts;
        EntityType expectedType;
        bool required = true;
    };

    struct SlotSchema {
        std::vector<SlotDefinition> slots;
        Intent intent;
    };

    class SlotRegistry {
    public:
        SlotRegistry();

        [[nodiscard]] SlotSchema const* find(Intent name) const;
        void registerSchema(SlotSchema schema);
        void clear();

    private:
        std::unordered_map<Intent, SlotSchema> m_schemas;
    };

    enum class SlotFillStatus {
        IDLE,
        AWAITING_SLOT,
        COMPLETE,
    };

    struct SlotFillResult {
        std::string nextPrompt;
        EntityMap filled;
        Intent intent;
        SlotFillStatus status;
    };

    class SlotFiller {
    public:
        SlotFiller(SlotRegistry const& registry);

        SlotFillResult begin(Intent intent, std::span<Entity const> entities);
        SlotFillResult continueFill(std::span<Entity const> entities);

        [[nodiscard]] bool isActive() const { return m_status != SlotFillStatus::IDLE; }
        [[nodiscard]] Intent activeIntent() const { return m_intent; }
        void reset();

    private:
        void absorb(std::span<Entity const> entities);
        [[nodiscard]] SlotDefinition const* nextMissing() const;
        std::string pickPrompt(SlotDefinition const& slot);
        SlotFillResult makeResult();

        SlotRegistry const& m_registry;
        EntityMap m_filled;
        SlotSchema const* m_schema = nullptr;
        Intent m_intent;
        SlotFillStatus m_status = SlotFillStatus::IDLE;
        size_t m_prevFilledCount = 0;
        size_t m_stalledTurns = 0;
        std::string m_lastMissingSlot;
        geode::utils::StringMap<size_t> m_promptCursor;
    };
}