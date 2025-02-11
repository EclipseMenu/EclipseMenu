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

    geode::Result<> Bot::save(const std::filesystem::path& path) {
        m_replay.author = utils::get<GJAccountManager>()->m_username;
        m_replay.duration = m_replay.inputs.size() > 0 ? m_replay.inputs[m_replay.inputs.size() - 1].frame / m_replay.framerate : 0;

        auto res = m_replay.exportData();

        if(res.isErr())
            return geode::Err(res.unwrapErr());
        geode::ByteVector data = res.unwrap();

        std::ofstream file(path, std::ios::binary);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();
        return geode::Ok();
    }

    Result<> Bot::load(const std::filesystem::path& path) {
        std::ifstream f(path, std::ios::binary);

        if (!f) {
            f.close();
            return Err("Failed to open file");
        }

        f.seekg(0, std::ios::end);
        size_t fileSize = f.tellg();
        f.seekg(0, std::ios::beg);

        geode::ByteVector data(fileSize);
        f.read(reinterpret_cast<char*>(data.data()), fileSize);
        f.close();

        gdr::Result<BotReplay> res = gdr::Err<BotReplay>("");

        if(std::equal(data.begin(), data.begin() + 3, "GDR"))
            res = BotReplay::importData(data);
        else
            res = gdr::convert<BotReplay, gdr::Input<>>(data);

        if (res.isErr())
            return geode::Err(res.unwrapErr());

        m_replay = res.unwrap();

        return Ok();
    }

}