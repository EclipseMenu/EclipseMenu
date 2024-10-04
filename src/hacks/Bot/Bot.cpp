#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/bot/bot.hpp>
#include <modules/keybinds/manager.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CheckpointObject.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Bot {

    static bot::Bot s_bot;

    void saveReplay() {
        std::filesystem::path replayDirectory = Mod::get()->getSaveDir() / "replays";

        if (!std::filesystem::exists(replayDirectory))
            std::filesystem::create_directory(replayDirectory);

        std::filesystem::path replayPath = replayDirectory / (config::get<std::string>("bot.replayname", "temp") + ".gdr");
        s_bot.save(replayPath);
    }

    void loadReplay() {
        std::filesystem::path replayPath = config::get<std::string>("bot.selectedreplay", "");
        s_bot.load(replayPath);
    }

    class Bot : public hack::Hack {
        void init() override {
            const auto updateBotState = [](int state) { s_bot.setState(static_cast<bot::State>(state)); };

            config::setIfEmpty("bot.state", 0);
            updateBotState(config::get<int>("bot.state", 0));

            auto tab = gui::MenuTab::find("Bot");

            tab->addRadioButton("Disabled", "bot.state", 0)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("Record", "bot.state", 1)->callback(updateBotState)->handleKeybinds();
            tab->addRadioButton("Playback", "bot.state", 2)->callback(updateBotState)->handleKeybinds();

            tab->addInputText("Replay Name", "bot.replayname");
            tab->addFilesystemCombo("Replays", "bot.selectedreplay", Mod::get()->getSaveDir() / "replays");
            tab->addButton("Save")->callback(saveReplay);
            tab->addButton("Load")->callback(loadReplay);
        }

        [[nodiscard]] bool isCheating() override { return config::get<int>("bot.state") != 0; } // TODO: add a check for if theres a macro loaded
        [[nodiscard]] const char* getId() const override { return "Bot"; }
    };

    REGISTER_HACK(Bot)

    class $modify(BotPLHook, PlayLayer) {
        bool init(GJGameLevel* gj, bool p1, bool p2) {
            bool result = PlayLayer::init(gj, p1, p2);
            s_bot.setLevelInfo(gdr::Level(gj->m_levelName, gj->m_levelID.value()));
            return result;
        }

        void resetLevel() {
            bool p1hold = m_player1->m_holdingButtons[1];
            bool p2hold = m_player2->m_holdingButtons[1];

            PlayLayer::resetLevel();

            if (s_bot.getState() == bot::State::RECORD) {
                //gd does this automatically for holding but not releases so we do it manually
                if(!p1hold)
                    s_bot.recordInput(m_gameState.m_currentProgress + 1, PlayerButton::Jump, true, false);
                if (m_gameState.m_isDualMode && m_levelSettings->m_twoPlayerMode && !p2hold)
                    s_bot.recordInput(m_gameState.m_currentProgress + 1, PlayerButton::Jump, false, false);
            }

            if (m_checkpointArray->count() > 0) return;

            s_bot.restart();

            if (s_bot.getState() == bot::State::PLAYBACK) return;

            s_bot.clearInputs();
        }

        CheckpointObject* markCheckpoint() {
            if (s_bot.getState() == bot::State::RECORD && (m_player1->m_isDead || m_player2->m_isDead))
                return nullptr;

            return PlayLayer::markCheckpoint();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            PlayLayer* playLayer = PlayLayer::get();

            if (s_bot.getState() != bot::State::RECORD || !playLayer)
                return PlayLayer::loadFromCheckpoint(checkpoint);

            s_bot.removeInputsAfter(checkpoint->m_gameState.m_currentProgress);

            PlayLayer::loadFromCheckpoint(checkpoint);
        }
    };

    class $modify(BotBGLHook, GJBaseGameLayer) {
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (s_bot.getState() != bot::State::PLAYBACK)
                return;

            std::optional<gdr::Input> input = std::nullopt;

            while ((input = s_bot.poll(m_gameState.m_currentProgress)) != std::nullopt) {
                GJBaseGameLayer::handleButton(input->down, (int) input->button, !input->player2);
            }
        }

        void handleButton(bool down, int button, bool player1) {
            GJBaseGameLayer::handleButton(down, button, player1);

            if (s_bot.getState() != bot::State::RECORD)
                return;

            std::optional<gdr::Input> lastInput = s_bot.findLastInputForPlayer(player1, (PlayerButton) button);

            if(lastInput && lastInput->down == down)
                return;

            s_bot.recordInput(m_gameState.m_currentProgress, (PlayerButton) button, !player1, down);
        }
    };

}