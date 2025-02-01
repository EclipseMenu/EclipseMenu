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

        std::string const& getName() const { return m_name; }
        std::function<bool()> const& getCallback() const { return m_isCheatActive; }

    private:
        std::string m_name;
        std::function<bool()> m_isCheatActive;
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

#endif // ECLIPSE_MODULES_HPP