#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include "bot.hpp"

using namespace geode::prelude;

namespace eclipse::bot {

    void Bot::setState(State state) {
        m_state = state;
    }

    void Bot::clearInputs() {
        m_replay.inputs.clear();
    }

    void Bot::restart() {
        m_inputIndex = 0;
    }

    void Bot::removeInputsAfter(int frame) {
        const auto check = [&](const gdr::Input &input) -> bool { return input.frame > frame; };
        m_replay.inputs.erase(std::remove_if (m_replay.inputs.begin(), m_replay.inputs.end(), check), m_replay.inputs.end());
    }

    void Bot::recordInput(int frame, PlayerButton button, bool player2, bool pressed) {
        gdr::Input input(frame, (int)button, player2, pressed);
        m_replay.inputs.push_back(input);
    }

    std::optional<gdr::Input> Bot::poll(int frame) {
        if(m_inputIndex >= m_replay.inputs.size())
            return std::nullopt;

        if(m_replay.inputs[m_inputIndex].frame <= frame)
            return m_replay.inputs[m_inputIndex++];
        
        return std::nullopt;
    }

}