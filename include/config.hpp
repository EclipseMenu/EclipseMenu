#pragma once
#ifndef ECLIPSE_CONFIG_HPP
#define ECLIPSE_CONFIG_HPP

#include <concepts>
#include <string>
#include <utility>
#include "events.hpp"

namespace eclipse::config {
    /// @brief Concept for supported types in the config system (bool, int, float, std::string)
    template <typename T>
    concept SupportedType = requires(T a) {
        std::same_as<T, bool> || std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, std::string> || std::same_as<T, std::string_view>;
    };
}

#ifndef ECLIPSE_DONT_DEFINE_IMPLS

namespace eclipse::config {
    /// @brief Get a config value by key from the session storage.
    /// @param key The key of the value.
    /// @param defaultValue The default value to return if the key doesn't exist.
    /// @return The value of the key or the default value if the key doesn't exist.
    template <SupportedType T>
    T get(std::string_view key, T defaultValue) {
        auto& vtable = __internal__::getVTable();
        if constexpr (std::is_same_v<T, bool>) {
            if (vtable.Config_getBool) {
                return vtable.Config_getBool(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (vtable.Config_getInt) {
                return vtable.Config_getInt(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (vtable.Config_getFloat) {
                return vtable.Config_getFloat(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (vtable.Config_getString) {
                return vtable.Config_getString(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            if (vtable.Config_getString) {
                return vtable.Config_getStringView(key, defaultValue);
            }
        }
        return defaultValue;
    }

    /// @brief Get a config value by key from the persistent storage (config.json).
    /// @param key The key of the value.
    /// @param defaultValue The default value to return if the key doesn't exist.
    /// @return The value of the key or the default value if the key doesn't exist.
    template <SupportedType T>
    T getInternal(std::string_view key, T defaultValue) {
        auto& vtable = __internal__::getVTable();
        if constexpr (std::is_same_v<T, bool>) {
            if (vtable.Config_getBoolInternal) {
                return vtable.Config_getBoolInternal(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (vtable.Config_getIntInternal) {
                return vtable.Config_getIntInternal(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (vtable.Config_getFloatInternal) {
                return vtable.Config_getFloatInternal(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (vtable.Config_getStringInternal) {
                return vtable.Config_getStringInternal(key, defaultValue);
            }
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            if (vtable.Config_getStringInternal) {
                return vtable.Config_getStringViewInternal(key, defaultValue);
            }
        }
        return defaultValue;
    }

    /// @brief Set a config value by key in the session storage.
    /// @param key The key of the value.
    /// @param value The value to set.
    template <SupportedType T>
    void set(std::string_view key, T value) {
        auto& vtable = __internal__::getVTable();
        if constexpr (std::is_same_v<T, bool>) {
            if (vtable.Config_setBool) {
                vtable.Config_setBool(key, value);
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (vtable.Config_setInt) {
                vtable.Config_setInt(key, value);
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (vtable.Config_setFloat) {
                vtable.Config_setFloat(key, value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (vtable.Config_setString) {
                vtable.Config_setString(key, std::move(value));
            }
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            if (vtable.Config_setStringView) {
                vtable.Config_setStringView(key, value);
            }
        }
    }

    /// @brief Set a config value by key in the persistent storage (config.json).
    /// @param key The key of the value.
    /// @param value The value to set.
    template <SupportedType T>
    void setInternal(std::string_view key, T value) {
        auto& vtable = __internal__::getVTable();
        if constexpr (std::is_same_v<T, bool>) {
            if (vtable.Config_setBoolInternal) {
                vtable.Config_setBoolInternal(key, value);
            }
        } else if constexpr (std::is_same_v<T, int>) {
            if (vtable.Config_setIntInternal) {
                vtable.Config_setIntInternal(key, value);
            }
        } else if constexpr (std::is_same_v<T, float>) {
            if (vtable.Config_setFloatInternal) {
                vtable.Config_setFloatInternal(key, value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (vtable.Config_setStringInternal) {
                vtable.Config_setStringInternal(key, std::move(value));
            }
        } else if constexpr (std::is_same_v<T, std::string_view>) {
            if (vtable.Config_setStringViewInternal) {
                vtable.Config_setStringViewInternal(key, value);
            }
        }
    }
}

#endif // ECLIPSE_DONT_DEFINE_IMPLS

#endif // ECLIPSE_CONFIG_HPP