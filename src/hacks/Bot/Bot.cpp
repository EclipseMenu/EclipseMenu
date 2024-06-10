#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include "modules/bot/bot.hpp"

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CheckpointObject.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Bot {

    static bot::Bot s_bot;

    class Bot : public hack::Hack {
        void init() override {

            const auto updateBotState = [](int state) { s_bot.setState(bot::State(state)); };

            auto tab = gui::MenuTab::find("Bot");
            tab->addRadioButton("Disabled", "bot.state", 0)->callback(updateBotState);
            tab->addRadioButton("Record", "bot.state", 1)->callback(updateBotState);
            tab->addRadioButton("Playback", "bot.state", 2)->callback(updateBotState);
        }

        void update() override {}
        [[nodiscard]] bool isCheating() override { return false; }
        [[nodiscard]] const char* getId() const override { return "Bot"; }
    };

    REGISTER_HACK(Bot)

    class $modify(PlayLayer) {

        void resetLevel() {
            PlayLayer::resetLevel();

            if(s_bot.getState() == bot::State::RECORD) {
                s_bot.recordInput(m_gameState.m_currentProgress, PlayerButton::Jump, true, false);
                s_bot.recordInput(m_gameState.m_currentProgress, PlayerButton::Jump, false, false);
            }

            if(m_checkpointArray->count() > 0) return;

            s_bot.restart();

            if(s_bot.getState() == bot::State::PLAYBACK) return;

            s_bot.clearInputs();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            PlayLayer* playLayer = PlayLayer::get();

            if(s_bot.getState() != bot::State::RECORD || !playLayer)
                return PlayLayer::loadFromCheckpoint(checkpoint);
            
            s_bot.removeInputsAfter(checkpoint->m_gameState.m_currentProgress);

            PlayLayer::loadFromCheckpoint(checkpoint);
        }
    };

    class $modify(GJBaseGameLayer) {
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if(s_bot.getState() != bot::State::PLAYBACK)
                return;

            std::optional<gdr::Input> input = std::nullopt;

            while((input = s_bot.poll(m_gameState.m_currentProgress)) != std::nullopt)
                GJBaseGameLayer::handleButton(input->down, (int)input->button, !input->player2);
        }

        void handleButton(bool down, int button, bool player1) {
            GJBaseGameLayer::handleButton(down, button, player1);

            if(s_bot.getState() != bot::State::RECORD)
                return;

            s_bot.recordInput(m_gameState.m_currentProgress, (PlayerButton)button, !player1, down);
        }
    };

}