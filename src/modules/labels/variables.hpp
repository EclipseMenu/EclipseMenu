#pragma once
#include <string>
#include <unordered_map>
#include <rift.hpp>

namespace eclipse::labels {

    /// @brief Class that represents a variable manager, used to store and retrieve variables.
    class VariableManager {
    public:
        using VarMap = std::unordered_map<std::string, rift::Value>;

        static VariableManager& get();

        /// @brief Set default variables (which usually don't change).
        void init();

        /// @brief Set a variable with the specified name and value.
        void setVariable(const std::string& name, const rift::Value& value);

        /// @brief Get the value of a variable with the specified name.
        [[nodiscard]] rift::Value getVariable(const std::string& name) const;

        /// @brief Check if a variable with the specified name exists.
        [[nodiscard]] bool hasVariable(const std::string& name) const;

        /// @brief Remove a variable with the specified name.
        void removeVariable(const std::string& name);

        /// @brief Get all variables.
        [[nodiscard]] const VarMap& getVariables() const { return m_variables; }

        /// @brief Re-fetch all variables from the game
        void refetch();

        /// @brief Update the FPS variable.
        void updateFPS();

    private:
        VarMap m_variables;
    };

}