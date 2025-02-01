#pragma once
#ifndef ECLIPSE_LABELS_HPP
#define ECLIPSE_LABELS_HPP

#include <string>
#include <string_view>
#include <concepts>
#include <Geode/loader/Event.hpp>
#include <matjson.hpp>

namespace eclipse::label {

    using null_t = std::monostate;

    /// @brief Concept for supported types in the label system (bool, int64_t, double, std::string, RiftNull, matjson::Value)
    template <typename T>
    concept SupportedType = requires(T a) {
        std::same_as<T, bool> || std::same_as<T, int64_t> || std::same_as<T, double> ||
        std::same_as<T, std::string> || std::same_as<T, null_t> || std::same_as<T, matjson::Value>;
    };

}

namespace eclipse::events {

    class FormatRiftStringEvent final : public geode::Event {
    public:
        explicit FormatRiftStringEvent(std::string_view source) : m_source(source) {}
        std::string_view getSource() const { return m_source; }
        std::string const& getResult() const { return m_result; }
        void setResult(std::string_view result) { m_result = result; }
    private:
        std::string_view m_source;
        std::string m_result;
    };

    template <label::SupportedType T>
    class GetRiftVariableEvent final : public geode::Event {
    public:
        explicit GetRiftVariableEvent(std::string_view name)
            : m_name(name), m_result(geode::Err("Unknown error")) {}
        std::string_view getName() const { return m_name; }
        geode::Result<T> const& getResult() const { return m_result; }
        void setResult(geode::Result<T> result) { m_result = std::move(result); }

    private:
        std::string_view m_name;
        geode::Result<T> m_result;
    };

    template <label::SupportedType T>
    class SetRiftVariableEvent final : public geode::Event {
    public:
        explicit SetRiftVariableEvent(std::string_view name, T value) : m_name(name), m_value(value) {}
        std::string_view getName() const { return m_name; }
        T getValue() const { return m_value; }
    private:
        std::string_view m_name;
        T m_value;
    };
}

namespace eclipse::label {
    /// @brief Formats a string using the RIFT syntax and variables provided by Eclipse.
    /// @param fmt The format string to use. (see RIFT documentation)
    /// @return The formatted string.
    inline std::string format(std::string_view fmt) {
        auto event = events::FormatRiftStringEvent(fmt);
        event.post();
        return event.getResult();
    }

    /// @brief Gets a variable from the RIFT system.
    /// @param name The name of the variable.
    /// @return Result containing the variable value or an error.
    template <SupportedType T>
    geode::Result<T> getVariable(std::string_view name) {
        auto event = events::GetRiftVariableEvent<T>(name);
        event.post();
        return event.getResult();
    }

    /// @brief Sets a variable in the RIFT system.
    /// @param name The name of the variable.
    /// @param value The value to set.
    template <SupportedType T>
    void setVariable(std::string_view name, T value) {
        events::SetRiftVariableEvent<T>(name, value).post();
    }
}

#endif