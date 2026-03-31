#include "Slots.hpp"

namespace eclipse::ai {
    SlotRegistry::SlotRegistry() = default;

    SlotSchema const* SlotRegistry::find(Intent name) const {
        auto it = m_schemas.find(name);
        return it != m_schemas.end() ? &it->second : nullptr;
    }

    void SlotRegistry::registerSchema(SlotSchema schema) {
        m_schemas.emplace(schema.intent, std::move(schema));
    }

    void SlotRegistry::clear() {
        m_schemas.clear();
    }

    SlotFiller::SlotFiller(SlotRegistry const& registry)
        : m_registry(registry) {}

    SlotFillResult SlotFiller::begin(Intent intent, std::span<Entity const> entities) {
        this->reset();
        m_schema = m_registry.find(intent);
        if (!m_schema) {
            return { {}, {}, intent, SlotFillStatus::COMPLETE };
        }

        m_intent = intent;
        this->absorb(entities);
        return this->makeResult();
    }

    SlotFillResult SlotFiller::continueFill(std::span<Entity const> entities) {
        if (!isActive()) {
            return { {}, {}, Intent::INVALID, SlotFillStatus::IDLE };
        }

        this->absorb(entities);
        return this->makeResult();
    }

    void SlotFiller::reset() {
        m_filled.clear();
        m_intent = Intent::INVALID;
        m_schema = nullptr;
        m_status = SlotFillStatus::IDLE;
        m_prevFilledCount = 0;
        m_stalledTurns = 0;
        m_lastMissingSlot.clear();
        m_promptCursor.clear();
    }

    void SlotFiller::absorb(std::span<Entity const> entities) {
        if (!m_schema) return;
        for (auto const& slot : m_schema->slots) {
            if (m_filled.contains(slot.name)) continue;
            for (auto const& entity : entities) {
                if (entity.type == slot.expectedType) {
                    m_filled.emplace(slot.name, entity);
                    break;
                }
            }
        }
    }

    SlotDefinition const* SlotFiller::nextMissing() const {
        if (!m_schema) return nullptr;
        for (auto const& slot : m_schema->slots) {
            if (slot.required && !m_filled.contains(slot.name)) {
                return &slot;
            }
        }
        return nullptr;
    }

    std::string SlotFiller::pickPrompt(SlotDefinition const& slot) {
        if (slot.prompts.empty()) return {};
        if (slot.prompts.size() == 1) return slot.prompts.front();

        using geode::utils::random::generate;

        auto pick = generate(0, slot.prompts.size());
        if (auto it = m_promptCursor.find(slot.name); it != m_promptCursor.end() && pick == it->second) {
            pick = (it->second + 1 + generate(0, slot.prompts.size() - 1)) % slot.prompts.size();
        }

        m_promptCursor[slot.name] = pick;
        return slot.prompts[pick];
    }

    SlotFillResult SlotFiller::makeResult() {
        if (auto missing = this->nextMissing()) {
            if (m_lastMissingSlot == missing->name && m_filled.size() == m_prevFilledCount) {
                ++m_stalledTurns;
            } else {
                m_stalledTurns = 0;
            }
            m_lastMissingSlot = missing->name;
            m_prevFilledCount = m_filled.size();

            if (m_stalledTurns >= 2) {
                m_status = SlotFillStatus::COMPLETE;
                return { {}, m_filled, m_intent, m_status };
            }

            m_status = SlotFillStatus::AWAITING_SLOT;
            return { this->pickPrompt(*missing), {}, m_intent, m_status };
        }

        m_status = SlotFillStatus::COMPLETE;
        m_prevFilledCount = m_filled.size();
        m_stalledTurns = 0;
        m_lastMissingSlot.clear();
        return { {}, m_filled, m_intent, m_status };
    }
}