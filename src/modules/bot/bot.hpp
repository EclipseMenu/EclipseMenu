#pragma once

#include <gdr_convert.hpp>

namespace eclipse::bot {

    enum class State {
        DISABLED,
        RECORD,
        PLAYBACK
    };

    struct BotReplay : gdr::Replay<BotReplay, gdr::Input<>> {
        BotReplay() : Replay("EclipseBot", 1) {}
    };

    class Bot {
    public:

        /// @brief Set the current state of the bot.
        /// @param state State of the bot.
        void setState(State state);

        /// @brief Get the current state of the bot.
        [[nodiscard]] State getState() const { return m_state; }

        [[nodiscard]] size_t getInputCount() const { return m_replay.inputs.size(); }

        [[nodiscard]] float getFramerate() const { return m_replay.framerate; }

        void setFramerate(float framerate) { m_replay.framerate = framerate; }

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
        [[nodiscard]] std::optional<gdr::Input<>> poll(int frame);

        /// @brief Saves the current replay.
        /// @param path Path to save the replay to.
        geode::Result<> save(const std::filesystem::path& path);

        /// @brief Loads a replay from the given path.
        /// @param path Path to load the replay from.
        geode::Result<> load(const std::filesystem::path& path);

        /// @brief Loads a replay from a data buffer.
        /// @param data Data to load the replay from.
        geode::Result<> load(std::span<uint8_t> data);

        /// @brief Sets the level info.
        /// @param levelInfo The level info.
        void setLevelInfo(const gdr::Level& levelInfo);

        /// @brief Sets platformer.
        /// @param platformer Platformer.
        void setPlatformer(bool platformer);

        [[nodiscard]] std::optional<gdr::Input<>> getPrevious(bool player1);

    protected:
        State m_state = State::DISABLED;
        BotReplay m_replay;
        uint32_t m_inputIndex = 0;
    };

}