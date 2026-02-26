#pragma once
#ifndef ECLIPSE_MODULES_HPP
#define ECLIPSE_MODULES_HPP

#include "events.hpp"
#include <functional>
#include <memory>
#include <string>
#include <utility>

namespace eclipse::modules {
    /// @brief Register a cheat with the given name and callback.
    /// @param name The name of the cheat (how it will show up in cheat indicator list).
    /// @param isCheatActive The callback to check if the cheat is active.
    inline void registerCheat(std::string name, geode::Function<bool()> isCheatActive) {
        // events::RegisterCheatEvent(name, isCheatActive).post();
        auto& vtable = __internal__::getVTable();
        if (vtable.RegisterCheat) {
            vtable.RegisterCheat(std::move(name), std::move(isCheatActive));
        }
    }
}

namespace eclipse {
    /// @brief Load a replay from a file path.
    /// @param path The path to the replay file.
    inline geode::Result<> loadReplay(std::filesystem::path const& path) {
        auto& vtable = __internal__::getVTable();
        if (vtable.LoadReplay) {
            return vtable.LoadReplay(path);
        }
        return geode::Err("function not available");
    }

    /// @brief Load a replay from raw data.
    /// @param data The raw data of the replay file.
    inline geode::Result<> loadReplay(std::span<uint8_t> data) {
        auto& vtable = __internal__::getVTable();
        if (vtable.LoadReplayFromData) {
            return vtable.LoadReplayFromData(data);
        }
        return geode::Err("function not available");
    }

    /// @brief Check if any cheats are enabled.
    /// @return True if any cheats are enabled, false otherwise.
    inline bool hasCheatsEnabled() {
        auto& vtable = __internal__::getVTable();
        if (vtable.CheckCheatsEnabled) {
            return vtable.CheckCheatsEnabled();
        }
        return false;
    }

    /// @brief Check if the player has cheated in the current attempt.
    /// @return True if the player has cheated in the current attempt, false otherwise.
    inline bool hasCheatedInAttempt() {
        auto& vtable = __internal__::getVTable();
        if (vtable.CheckCheatedInAttempt) {
            return vtable.CheckCheatedInAttempt();
        }
        return false;
    }

    /// @brief Get a list of enabled cheating modules.
    /// @return A vector of HackingModule representing the enabled cheats.
    inline std::vector<HackingModule> getEnabledCheats() {
        auto& vtable = __internal__::getVTable();
        if (vtable.GetHackingModules) {
            return vtable.GetHackingModules();
        }
        return {};
    }
}

#endif // ECLIPSE_MODULES_HPP
