#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/utils/GameCheckpoint.hpp>

#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace eclipse::Hacks::Level {
    class $hack(PracticeFix) {
        static bool shouldEnable() {
            return config::get<bool>("bot.practicefix", false) || config::get<int>("bot.state", 0) == 1;
        }

        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("bot.practicefix")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Practice Fix"; }
    };

    REGISTER_HACK(PracticeFix)

    class CheckpointData {
    public:
        CheckpointData() = default;

        CheckpointData(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1 = utils::FixPlayerCheckpoint(player1);
            if (player2)
                m_checkpointPlayer2 = utils::FixPlayerCheckpoint(player2);
        }

        void apply(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1.apply(player1);
            if (player2)
                m_checkpointPlayer2.apply(player2);
        }

    private:
        utils::FixPlayerCheckpoint m_checkpointPlayer1;
        utils::FixPlayerCheckpoint m_checkpointPlayer2;
    };

    class $modify(FixPlayLayer, PlayLayer) {
        struct Fields {
            std::unordered_map<CheckpointObject*, CheckpointData> m_checkpoints;
        };

        void resetLevel() {
            if (m_checkpointArray->count() <= 0)
                m_fields->m_checkpoints.clear();

            PlayLayer::resetLevel();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            if (PracticeFix::shouldEnable()) {
                auto fields = m_fields.self();
                if (fields->m_checkpoints.contains(checkpoint)) {
                    PlayLayer::loadFromCheckpoint(checkpoint);

                    CheckpointData& data = fields->m_checkpoints[checkpoint];
                    data.apply(m_player1, m_gameState.m_isDualMode ? m_player2 : nullptr);

                    return;
                }
            }

            PlayLayer::loadFromCheckpoint(checkpoint);
        }

        CheckpointObject* createCheckpoint() {
            auto checkpoint = PlayLayer::createCheckpoint();
            if (!checkpoint || !PracticeFix::shouldEnable())
                return checkpoint;

            if (m_gameState.m_currentProgress > 0) {
                CheckpointData data(m_player1, m_gameState.m_isDualMode ? m_player2 : nullptr);
                m_fields->m_checkpoints[checkpoint] = std::move(data);
            }

            return checkpoint;
        }

        void removeCheckpoint(bool first) {
            // remove the checkpoint from the map first
            CheckpointObject* checkpoint = nullptr;
            if (m_checkpointArray->count()) {
                if (first) checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->objectAtIndex(0));
                else checkpoint = static_cast<CheckpointObject*>(m_checkpointArray->lastObject());
            }

            auto fields = m_fields.self();
            if (checkpoint && fields->m_checkpoints.contains(checkpoint)) {
                fields->m_checkpoints.erase(checkpoint);
            }

            PlayLayer::removeCheckpoint(first);
        }
    };
}
