#pragma once

#include <gdr/gdr.hpp>

namespace eclipse::bot {

    enum class State {
        DISABLED,
        RECORD,
        PLAYBACK
    };

    struct BotReplay : public gdr::Replay<BotReplay, gdr::Input> {
        BotReplay() : Replay("Bot", "1.0") {}
    };

    class Bot {
    public:

        /// @brief Set the current state of the bot.
        /// @param state State of the bot.
        void setState(State state);

        /// @brief Get the current state of the bot.
        [[nodiscard]] State getState() const { return m_state; }

        [[nodiscard]] size_t getInputCount() const { return m_replay.inputs.size(); }

        /// @brief Removes all inputs from the currently loaded replay.
        void clearInputs();

        /// @brief Restarts the replay from the beginning.
        void restart();

        /// @brief Removes all inputs from the currently loaded replay, up to the given frame.
        /// @param frame Frame to stop at.
        void removeInputsAfter(int frame);
        
        /// @brief Add an input to the currently loaded replay.
        void recordInput(int frame, PlayerButton button, bool player2, bool pressed);

        /// @brief Get the next available input from the currently loaded replay.
        /// @param frame The current frame of the playback.
        /// @return The next input from the currently loaded replay, if available.
        [[nodiscard]] std::optional<gdr::Input> poll(int frame);

        /// @brief Saves the current replay.
        /// @param path Path to save the replay to.
        void save(std::filesystem::path path);

        /// @brief Loads a replay from the given path.
        /// @param path Path to load the replay from.
        geode::Result<> load(std::filesystem::path path);

        /// @brief Sets the level info.
        /// @param levelInfo The level info.
        void setLevelInfo(gdr::Level levelInfo);

        [[nodiscard]] std::optional<gdr::Input> getPrevious(bool player1);

    protected:
        State m_state = State::DISABLED;
        BotReplay m_replay;
        uint32_t m_inputIndex = 0;
    };

}