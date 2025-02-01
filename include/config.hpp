#pragma once
#ifndef ECLIPSE_CONFIG_HPP
#define ECLIPSE_CONFIG_HPP

#include <concepts>
#include <string>
#include <utility>
#include <Geode/loader/Event.hpp>

namespace eclipse::config {
    /// @brief Concept for supported types in the config system (bool, int, float, std::string)
    template <typename T>
    concept SupportedType = requires(T a) {
        std::same_as<T, bool> || std::same_as<T, int> || std::same_as<T, float> || std::same_as<T, std::string>;
    };
}

namespace eclipse::events {
    template <config::SupportedType T>
    class RequestConfigValueEvent : public geode::Event {
    public:
        explicit RequestConfigValueEvent(std::string key, bool internal = false)
            : m_key(std::move(key)), m_useInternal(internal) {}
        const std::string& getKey() const { return m_key; }
        T getValue() const { return m_value; }
        bool hasValue() const { return m_hasValue; }
        bool getUseInternal() const { return m_useInternal; }

        void setValue(T value) {
            m_value = value;
            m_hasValue = true;
        }

    private:
        std::string m_key;
        T m_value;
        bool m_useInternal = false;
        bool m_hasValue = false;
    };

    template <config::SupportedType T>
    class SetConfigValueEvent : public geode::Event {
    public:
        SetConfigValueEvent(std::string key, T value, bool internal = false)
            : m_key(std::move(key)), m_value(value), m_useInternal(internal) {}

        std::string const& getKey() const { return m_key; }
        T getValue() const { return m_value; }
        bool getUseInternal() const { return m_useInternal; }

    private:
        std::string m_key;
        T m_value;
        bool m_useInternal = false;
    };

}

namespace eclipse::config {

    /// @brief Get a config value by key from the session storage.
    /// @param key The key of the value.
    /// @param defaultValue The default value to return if the key doesn't exist.
    /// @return The value of the key or the default value if the key doesn't exist.
    template <SupportedType T>
    T get(std::string key, T defaultValue) {
        events::RequestConfigValueEvent<T> event(key);
        event.post();
        if (event.hasValue())
            return event.getValue();
        return defaultValue;
    }

    /// @brief Get a config value by key from the persistent storage (config.json).
    /// @param key The key of the value.
    /// @param defaultValue The default value to return if the key doesn't exist.
    /// @return The value of the key or the default value if the key doesn't exist.
    template <SupportedType T>
    T getInternal(std::string key, T defaultValue) {
        events::RequestConfigValueEvent<T> event(key, true);
        event.post();
        if (event.hasValue())
            return event.getValue();
        return defaultValue;
    }

    /// @brief Set a config value by key in the session storage.
    /// @param key The key of the value.
    /// @param value The value to set.
    template <SupportedType T>
    void set(std::string key, T value) {
        events::SetConfigValueEvent<T>(key, value, false).post();
    }

    /// @brief Set a config value by key in the persistent storage (config.json).
    /// @param key The key of the value.
    /// @param value The value to set.
    template <SupportedType T>
    void setInternal(std::string key, T value) {
        events::SetConfigValueEvent<T>(key, value, true).post();
    }
}

#endif // ECLIPSE_CONFIG_HPP