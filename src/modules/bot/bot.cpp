#include <algorithm>
#include <gdr/gdr.hpp>
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>

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
        std::erase_if(m_replay.inputs, [&](const gdr::Input<>& input) -> bool { return input.frame > frame; });
    }

    void Bot::recordInput(int frame, PlayerButton button, bool player2, bool pressed) {
        m_replay.inputs.emplace_back(frame, static_cast<int>(button), player2, pressed);
    }

    std::optional<gdr::Input<>> Bot::poll(int frame) {
        if (m_inputIndex >= m_replay.inputs.size())
            return std::nullopt;

        if (m_replay.inputs[m_inputIndex].frame <= frame)
            return m_replay.inputs[m_inputIndex++];

        return std::nullopt;
    }

    std::optional<gdr::Input<>> Bot::getPrevious(bool player1) {
        for (int i = m_inputIndex - 2; i >= 0; i--) {
            if (m_replay.inputs[i].player2 == !player1)
                return m_replay.inputs[i];
        }

        return std::nullopt;
    }

    void Bot::setLevelInfo(const gdr::Level& levelInfo) {
        m_replay.levelInfo = levelInfo;
    }

    void Bot::setPlatformer(bool platformer) {
        m_replay.platformer = platformer;
    }

    Result<> Bot::save(const std::filesystem::path& path) {
        m_replay.author = utils::get<GJAccountManager>()->m_username;
        m_replay.duration = !m_replay.inputs.empty() ? m_replay.inputs[m_replay.inputs.size() - 1].frame / m_replay.framerate : 0;

        auto res = m_replay.exportData();

        if(res.isErr())
            return Err(std::move(res).unwrapErr());

        auto data = std::move(res).unwrap();
        return file::writeBinarySafe(path, data);
    }

    Result<> Bot::load(const std::filesystem::path& path) {
        auto res = file::readBinary(path);
        if (res.isErr())
            return Err(std::move(res).unwrapErr());

        auto fileData = std::move(res).unwrap();
        return load(fileData);
    }

    Result<> Bot::load(std::span<uint8_t> data) {
        if (data.size() < 3)
            return Err("Data is too small to be a valid replay");

        gdr::Result<BotReplay> res = gdr::Err<BotReplay>("");
        if(std::equal(data.begin(), data.begin() + 3, "GDR"))
            res = BotReplay::importData(data);
        else
            res = gdr::convert<BotReplay, gdr::Input<>>(data);

        if (res.isErr())
            return Err(std::move(res).unwrapErr());

        m_replay = std::move(res).unwrap();

        return Ok();
    }
}
