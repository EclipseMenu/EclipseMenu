#pragma once
#include <rift.hpp>
#include <string>
#include <unordered_map>

// Quickly assign a value to a label variable for debugging purposes.
#define DEBUG_VAR(name, value) eclipse::labels::VariableManager::get().setVariable(name, rift::Value::from(value))

namespace eclipse::labels {
    /// @brief Class that represents a variable manager, used to store and retrieve variables.
    class VariableManager {
    public:
        static VariableManager& get();

        /// @brief Set default variables (which usually don't change).
        void init();

        /// @brief Set a variable with the specified name and value.
        void setVariable(std::string name, rift::Value&& value);

        /// @brief Get the value of a variable with the specified name.
        [[nodiscard]] rift::Value getVariable(std::string const& name) const;

        /// @brief Check if a variable with the specified name exists.
        [[nodiscard]] bool hasVariable(std::string const& name) const;

        /// @brief Remove a variable with the specified name.
        void removeVariable(std::string const& name);

        /// @brief Get all variables.
        [[nodiscard]] rift::Object const& getVariables() const { return m_variables; }

        /// @brief Re-fetch all variables from the game
        void refetch();

        /// @brief Update the FPS variable.
        void updateFPS();

    private:
        void fetchGeneralData();
        void fetchTimeData();
        void fetchHacksData();
        void fetchLevelData(GJGameLevel* level);
        void fetchPlayerData(PlayerObject* player, bool isPlayer2);
        void fetchGameplayData(GJBaseGameLayer* gameLayer);

        rift::Object m_variables;
    };
}
