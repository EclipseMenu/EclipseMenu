#pragma once

#include <nlohmann/json.hpp>
#include <fmt/format.h>
#include <string>
#include <vector>

namespace eclipse::config {

    /// @brief Get the container for the configuration file.
    nlohmann::json& getStorage();

    /// @brief Get the container for temporary storage.
    nlohmann::json& getTempStorage();

    /// @brief Load the configuration file.
    void load();

    /// @brief Save the configuration file.
    void save();

    /// @brief Save the configuration file as a profile, to be loaded later.
    /// @param profile Name of the profile to save.
    void saveProfile(std::string_view profile);

    /// @brief Load a configuration profile from profile name.
    /// @param profile Name of the profile to load.
    void loadProfile(std::string_view profile);

    /// @brief Delete a configuration profile.
    /// @param profile Name of the profile to delete.
    void deleteProfile(std::string_view profile);

    /// @brief Get a list of all configuration profiles.
    std::vector<std::string> getProfiles();

    /// @brief Check if a key exists in the configuration.
    /// @param key Key to check.
    /// @return True if the key exists in the configuration.
    inline bool has(std::string_view key) {
        return getStorage().contains(key);
    }

    /// @brief Get a value by key from the configuration.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the configuration or the default value if the key does not exist.
    template<typename T>
    inline T get(std::string_view key, const T& defaultValue) {
        if (!has(key))
            return defaultValue;

        return getStorage().at(key).get<T>();
    }

    /// @brief Get a value by key from the configuration.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the configuration.
    template<typename T>
    inline T get(std::string_view key) {
        if (!has(key))
            throw std::runtime_error(fmt::format("Key '{}' does not exist", key));

        return getStorage().at(key).get<T>();
    }

    /// @brief Set a value by key in the configuration.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    inline void set(std::string_view key, const T& value) {
        getStorage()[key] = value;
    }

    /// @brief Check if the value is of the specified type.
    /// @tparam T Type to check.
    /// @param key Key to check.
    /// @return True if the value is of the specified type.
    template<typename T>
    inline bool is(std::string_view key) {
        if (!has(key))
            return false;

        try {
            get<T>(key);
            return true;
        } catch (...) {
            return false;
        }
    }

    /// @brief Set a value by key in the configuration if the key does not exist.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    inline void setIfEmpty(std::string_view key, const T& value) {
        if (!has(key))
            set(key, value);
    }

    /// @brief Check if a key exists in the temporary storage.
    /// @param key Key to check.
    /// @return True if the key exists in the temporary storage.
    inline bool hasTemp(std::string_view key) {
        return getTempStorage().contains(key);
    }

    /// @brief Get a value by key from the temporary storage.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the temporary storage or the default value if the key does not exist.
    template<typename T>
    inline T getTemp(std::string_view key, const T& defaultValue) {
        if (!hasTemp(key))
            return defaultValue;

        return getTempStorage().at(key).get<T>();
    }

    /// @brief Get a value by key from the temporary storage.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the temporary storage.
    template<typename T>
    inline T getTemp(std::string_view key) {
        if (!hasTemp(key))
            throw std::runtime_error(fmt::format("Key '{}' does not exist", key));

        return getTempStorage().at(key).get<T>();
    }

    /// @brief Set a value by key in the temporary storage.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    inline void setTemp(std::string_view key, const T& value) {
        getTempStorage()[key] = value;
    }
}