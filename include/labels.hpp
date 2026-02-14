#pragma once
#ifndef ECLIPSE_LABELS_HPP
#define ECLIPSE_LABELS_HPP

#include <concepts>
#include <matjson.hpp>
#include <string>
#include <string_view>

namespace eclipse::label {
    using null_t = std::nullptr_t;

    /// @brief Concept for supported types in the label system (bool, int64_t, double, std::string, RiftNull, matjson::Value)
    template <typename T>
    concept SupportedType = requires(T a) {
        std::same_as<T, bool> || std::same_as<T, int64_t> || std::same_as<T, double> ||
        std::same_as<T, std::string> || std::same_as<T, null_t> || std::same_as<T, matjson::Value>;
    };

    /// @brief Formats a string using the RIFT syntax and variables provided by Eclipse.
    /// @param fmt The format string to use. (see RIFT documentation)
    /// @return The formatted string.
    inline std::string format(std::string_view fmt) {
        auto& vtable = __internal__::getVTable();
        if (vtable.FormatRiftString) {
            return vtable.FormatRiftString(fmt);
        }
        return std::string(fmt);
    }

    /// @brief Gets a variable from the RIFT system.
    /// @param name The name of the variable.
    /// @return Result containing the variable value or an error.
    template <SupportedType T>
    geode::Result<T> getVariable(std::string_view name) {
        auto& vtable = __internal__::getVTable();
        if constexpr (std::same_as<T, std::string>) {
            if (vtable.GetRiftVariableString) {
                return vtable.GetRiftVariableString(name);
            }
        } else if constexpr (std::same_as<T, bool>) {
            if (vtable.GetRiftVariableBool) {
                return vtable.GetRiftVariableBool(name);
            }
        } else if constexpr (std::same_as<T, int64_t>) {
            if (vtable.GetRiftVariableInt) {
                return vtable.GetRiftVariableInt(name);
            }
        } else if constexpr (std::same_as<T, double>) {
            if (vtable.GetRiftVariableDouble) {
                return vtable.GetRiftVariableDouble(name);
            }
        } else if constexpr (std::same_as<T, null_t>) {
            if (vtable.GetRiftVariableNull) {
                return vtable.GetRiftVariableNull(name);
            }
        } else if constexpr (std::same_as<T, matjson::Value>) {
            if (vtable.GetRiftVariableObject) {
                return vtable.GetRiftVariableObject(name);
            }
        }
        return geode::Err("Unsupported type or function not available");
    }

    /// @brief Sets a variable in the RIFT system.
    /// @param name The name of the variable.
    /// @param value The value to set.
    template <SupportedType T>
    void setVariable(std::string name, T value) {
        auto& vtable = __internal__::getVTable();
        if constexpr (std::same_as<T, std::string>) {
            if (vtable.SetRiftVariableString) {
                vtable.SetRiftVariableString(std::move(name), std::move(value));
            }
        } else if constexpr (std::same_as<T, bool>) {
            if (vtable.SetRiftVariableBool) {
                vtable.SetRiftVariableBool(std::move(name), value);
            }
        } else if constexpr (std::same_as<T, int64_t>) {
            if (vtable.SetRiftVariableInt) {
                vtable.SetRiftVariableInt(std::move(name), value);
            }
        } else if constexpr (std::same_as<T, double>) {
            if (vtable.SetRiftVariableDouble) {
                vtable.SetRiftVariableDouble(std::move(name), value);
            }
        } else if constexpr (std::same_as<T, null_t>) {
            if (vtable.SetRiftVariableNull) {
                vtable.SetRiftVariableNull(std::move(name), value);
            }
        } else if constexpr (std::same_as<T, matjson::Value>) {
            if (vtable.SetRiftVariableObject) {
                vtable.SetRiftVariableObject(std::move(name), std::move(value));
            }
        }
    }
}

#endif
