#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/utils/GameCheckpoint.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CheckpointObject.hpp>

using namespace geode::prelude;

namespace eclipse::Hacks::Level {

    class PracticeFix : public hack::Hack {
    public:
        static bool shouldEnable() {
            return config::get<bool>("bot.practicefix", false) || config::get<int>("bot.state", 0) == 1;
        }
        
    private:
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Practice Fix", "bot.practicefix")
                ->setDescription("Properly saves and restores the player's data when respawning from a checkpoint.");
        }

        [[nodiscard]] bool isCheating() override { return false; }
        [[nodiscard]] const char* getId() const override { return "Practice Fix"; }
    };

    REGISTER_HACK(PracticeFix)

    class CheckpointData {
    public:
        CheckpointData() = default;

        CheckpointData(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1 = eclipse::utils::FixPlayerCheckpoint(player1);
            if (player2)
                m_checkpointPlayer2 = utils::FixPlayerCheckpoint(player2);
        }

        void apply(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1.apply(player1);
            if (player2)
                m_checkpointPlayer2.apply(player2);
        }

    private:
        eclipse::utils::FixPlayerCheckpoint m_checkpointPlayer1;
        eclipse::utils::FixPlayerCheckpoint m_checkpointPlayer2;
    };

    class $modify(FixPlayLayer, PlayLayer) {
        struct Fields {
            std::unordered_map<CheckpointObject*, CheckpointData> m_checkpoints;
        };

        void onQuit() {
            m_fields->m_checkpoints.clear();
            PlayLayer::onQuit();
        }

        void resetLevel() {
            if (m_checkpointArray->count() <= 0)
                m_fields->m_checkpoints.clear();

            PlayLayer::resetLevel();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get());

            if (PracticeFix::shouldEnable() && playLayer->m_fields->m_checkpoints.contains(checkpoint)) {
                PlayLayer::loadFromCheckpoint(checkpoint);

                CheckpointData& data = playLayer->m_fields->m_checkpoints[checkpoint];
                data.apply(playLayer->m_player1, playLayer->m_gameState.m_isDualMode ? playLayer->m_player2 : nullptr);

                return;
            }

            PlayLayer::loadFromCheckpoint(checkpoint);
        }

    };

    class $modify(PracticeFixLELHook, LevelEditorLayer) {
        bool init(GJGameLevel* level, bool unk) {
            bool result = LevelEditorLayer::init(level, unk);

            if (auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get()))
                playLayer->m_fields->m_checkpoints.clear();
            
            return result;
        }
    };

    class $modify(PracticeFixCOHook, CheckpointObject) {
#ifdef GEODE_IS_ANDROID
        static CheckpointObject* create() { // this is so dumb
            auto result = CheckpointObject::create();
#else 
        bool init() override {
            auto result = CheckpointObject::init();
#endif

            if (!PracticeFix::shouldEnable())
                return result;

            auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get());

            if (playLayer->m_gameState.m_currentProgress > 0) {
                CheckpointData data(playLayer->m_player1, playLayer->m_gameState.m_isDualMode ? playLayer->m_player2 : nullptr);
#ifdef GEODE_IS_ANDROID
                playLayer->m_fields->m_checkpoints[result] = data;
#else
                playLayer->m_fields->m_checkpoints[(CheckpointObject*)this] = data;
#endif
            }

            return result;
        }
    };

}
