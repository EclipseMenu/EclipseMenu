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
    bool resetFrame = false;

    void saveReplay() {
        std::filesystem::path replayDirectory = Mod::get()->getSaveDir() / "replays";

        if (!std::filesystem::exists(replayDirectory))
		    std::filesystem::create_directory(replayDirectory);

        std::filesystem::path replayPath = replayDirectory / (config::get<std::string>("bot.replayname", "temp") + ".gdr");
        s_bot.save(replayPath);
    }

    void loadReplay() {
        std::filesystem::path replayPath = Mod::get()->getSaveDir() / "replays" / (config::get<std::string>("bot.replayname", "temp") + ".gdr");
        s_bot.load(replayPath);
    }

    class Bot : public hack::Hack {
        void init() override {
            const auto updateBotState = [](int state) { s_bot.setState(bot::State(state)); };

            config::setIfEmpty("bot.state", 0);

            auto tab = gui::MenuTab::find("Bot");
            tab->addRadioButton("Disabled", "bot.state", 0)->callback(updateBotState);
            tab->addRadioButton("Record", "bot.state", 1)->callback(updateBotState);
            tab->addRadioButton("Playback", "bot.state", 2)->callback(updateBotState);

            tab->addInputText("Replay Name", "bot.replayname");
            tab->addButton("Save")->callback(saveReplay);
            tab->addButton("Load")->callback(loadReplay);
        }

        [[nodiscard]] bool isCheating() override { return false; }
        [[nodiscard]] const char* getId() const override { return "Bot"; }
    };

    REGISTER_HACK(Bot)

    //temporary, player->m_isDead is wrong
    class $modify(BotPlayerObject, PlayerObject) {
        struct Fields {
            bool m_isDead = false;
        };

        void playerDestroyed(bool idk) {
            m_fields->m_isDead = true;
            PlayerObject::playerDestroyed(idk);
        }
    };

    class $modify(PlayLayer) {

        bool init(GJGameLevel *gj, bool p1, bool p2) {
            bool result = PlayLayer::init(gj, p1, p2);
            s_bot.setLevelInfo(gdr::Level(gj->m_levelName, gj->m_levelID.value()));
            return result;
        }

        void resetLevel() {
            resetFrame = true;
            PlayLayer::resetLevel();
            resetFrame = false;

            //temporary, player->m_isDead is wrong
            static_cast<BotPlayerObject*>(m_player1)->m_fields->m_isDead = false;
            static_cast<BotPlayerObject*>(m_player2)->m_fields->m_isDead = false;

            if (s_bot.getState() == bot::State::RECORD) {
                s_bot.recordInput(m_gameState.m_currentProgress + 1, PlayerButton::Jump, true, false);
                if (m_gameState.m_isDualMode)
                    s_bot.recordInput(m_gameState.m_currentProgress + 1, PlayerButton::Jump, false, false);
            }

            if (m_checkpointArray->count() > 0) return;

            s_bot.restart();

            if (s_bot.getState() == bot::State::PLAYBACK) return;

            s_bot.clearInputs();
        }

        CheckpointObject* markCheckpoint() {
            if (s_bot.getState() == bot::State::RECORD && (((BotPlayerObject*) m_player1)->m_fields->m_isDead || ((BotPlayerObject*) m_player2)->m_fields->m_isDead))
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

    class $modify(GJBaseGameLayer) {
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (s_bot.getState() != bot::State::PLAYBACK)
                return;

            std::optional<gdr::Input> input = std::nullopt;

            while ((input = s_bot.poll(m_gameState.m_currentProgress)) != std::nullopt)
                GJBaseGameLayer::handleButton(input->down, (int) input->button, !input->player2);
        }

        void handleButton(bool down, int button, bool player1) {
            GJBaseGameLayer::handleButton(down, button, player1);

            if (s_bot.getState() != bot::State::RECORD || resetFrame)
                return;

            s_bot.recordInput(m_gameState.m_currentProgress, (PlayerButton) button, !player1, down);
        }
    };

}