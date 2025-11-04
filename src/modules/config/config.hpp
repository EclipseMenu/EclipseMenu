#pragma once

#include <functional.hpp>
#include <string>
#include <vector>

#ifndef INCLUDE_NLOHMANN_JSON_HPP_
#include <nlohmann/json_fwd.hpp>
#include <nlohmann/detail/value_t.hpp>
#endif

namespace eclipse::config {
    /// @brief Get the container for the configuration file.
    nlohmann::json& getStorage();

    /// @brief Get the container for temporary storage.
    nlohmann::json& getTempStorage();

    /// @brief Used internally to trigger callbacks for a value change
    void executeCallbacks(std::string_view name);

    /// @brief Used internally to trigger callbacks for a value change in temporary storage
    void executeTempCallbacks(std::string_view name);

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
    bool has(std::string_view key);

    /// @brief Get a value by key from the configuration.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the configuration or the default value if the key does not exist.
    template<typename T>
    T get(std::string_view key, T const& defaultValue);

    /// @brief Get a value by key from the configuration.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the configuration.
    template<typename T>
    geode::Result<T> get(std::string_view key);

    /// @brief Set a value by key in the configuration.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    void set(std::string_view key, T const& value);

    /// @brief Erase a key from the configuration.
    /// @param key Key to erase.
    void erase(std::string_view key);

    /// @brief Get the type of value by key in the configuration.
    /// @param key Key to get the type of.
    /// @return Type of the value.
    nlohmann::detail::value_t getType(std::string_view key);

    /// @brief Check if the value is of the specified type.
    /// @tparam T Type to check.
    /// @param key Key to check.
    /// @return True if the value is of the specified type.
    template<typename T>
    bool is(std::string_view key);

    /// @brief Set a value by key in the configuration if the key does not exist.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    void setIfEmpty(std::string_view key, T const& value);

    /// @brief Registers a delegate which is called when a specific value in config is changed
    /// @param key Key of the value which should have a delegate
    /// @param callback Callback to call when value is changed
    /// @param first If true, the delegate will be added to the front of the list
    void addDelegate(std::string_view key, Function<void()>&& callback, bool first = false);

    /// @brief Registers a delegate which is called when a specific value in temp is changed
    /// @param key Key of the value which should have a delegate
    /// @param callback Callback to call when value is changed
    /// @param first If true, the delegate will be added to the front of the list
    void addTempDelegate(std::string_view key, Function<void()>&& callback, bool first = false);

    /// @brief Check if a key exists in the temporary storage.
    /// @param key Key to check.
    /// @return True if the key exists in the temporary storage.
    inline bool hasTemp(std::string_view key);

    /// @brief Get a value by key from the temporary storage.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @param defaultValue Default value to return if the key does not exist.
    /// @return Value from the temporary storage or the default value if the key does not exist.
    template<typename T>
    T getTemp(std::string_view key, T const& defaultValue);

    /// @brief Get a value by key from the temporary storage.
    /// @note If the key does not exist, it will throw an exception.
    /// @tparam T Type of the value to get.
    /// @param key Key to get the value from.
    /// @return Value from the temporary storage.
    template<typename T>
    geode::Result<T> getTemp(std::string_view key);

    /// @brief Set a value by key in the temporary storage.
    /// @tparam T Type of the value to set.
    /// @param key Key to set the value to.
    /// @param value Value to set.
    template<typename T>
    void setTemp(std::string_view key, T const& value);

    namespace __impl {
        template <size_t N>
        struct Key {
            char data[N];
            explicit(false) constexpr Key(char const* str) { std::copy_n(str, N, data); }
            constexpr operator std::string_view() const { return {data, N}; }
        };

        template <size_t N>
        Key(char const (&)[N]) -> Key<N - 1>;
    }

    /// @brief Cached version of get function to avoid multiple lookups.
    template <__impl::Key key, typename T>
    T get(T const& defaultValue = T{}) {
        static T value = (addDelegate(key, [] {
            value = get<T>(key, T{});
        }, true), get<T>(key, defaultValue));
        return value;
    }

    /// @brief Cached version of getTemp function to avoid multiple lookups.
    template <__impl::Key key, typename T>
    T getTemp(T const& defaultValue = T{}) {
        static T value = (addTempDelegate(key, [] {
            value = getTemp<T>(key, T{});
        }, true), getTemp<T>(key, defaultValue));
        return value;
    }
}
