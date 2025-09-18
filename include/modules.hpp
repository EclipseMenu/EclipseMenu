#pragma once
#ifndef ECLIPSE_MODULES_HPP
#define ECLIPSE_MODULES_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace eclipse::events {
    class RegisterCheatEvent final : public geode::Event {
    public:
        RegisterCheatEvent(std::string name, std::function<bool()> isCheatActive)
            : m_name(std::move(name)), m_isCheatActive(std::move(isCheatActive)) {}

        [[nodiscard]] std::string const& getName() const { return m_name; }
        [[nodiscard]] std::function<bool()> const& getCallback() const { return m_isCheatActive; }

    private:
        std::string m_name;
        std::function<bool()> m_isCheatActive;
    };

    class LoadReplayEvent final : public geode::Event {
    public:
        explicit LoadReplayEvent(std::filesystem::path const& path) : m_path(&path) {}
        explicit LoadReplayEvent(std::span<uint8_t> data) : m_data(data) {}

        [[nodiscard]] std::filesystem::path const* getPath() const { return m_path; }
        [[nodiscard]] std::span<uint8_t> getData() const { return m_data; }
        [[nodiscard]] geode::Result<> getResult() const && { return m_result; }
        void setResult(geode::Result<>&& result) { m_result = std::move(result); }

    private:
        std::filesystem::path const* m_path = nullptr;
        std::span<uint8_t> m_data;
        geode::Result<> m_result = geode::Err("Unknown error");
    };
}

namespace eclipse::modules {
    /// @brief Register a cheat with the given name and callback.
    /// @param name The name of the cheat (how it will show up in cheat indicator list).
    /// @param isCheatActive The callback to check if the cheat is active.
    inline void registerCheat(const std::string& name, const std::function<bool()>& isCheatActive) {
        events::RegisterCheatEvent(name, isCheatActive).post();
    }
}

namespace eclipse {
    /// @brief Load a replay from a file path.
    /// @param path The path to the replay file.
    inline geode::Result<> loadReplay(std::filesystem::path const& path) {
        events::LoadReplayEvent event(path);
        event.post();
        return std::move(event).getResult();
    }

    /// @brief Load a replay from raw data.
    /// @param data The raw data of the replay file.
    inline geode::Result<> loadReplay(std::span<uint8_t> data) {
        events::LoadReplayEvent event(data);
        event.post();
        return std::move(event).getResult();
    }
}

#endif // ECLIPSE_MODULES_HPP