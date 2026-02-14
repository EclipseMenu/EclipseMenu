#pragma once
#ifndef ECLIPSE_EVENTS_HPP
#define ECLIPSE_EVENTS_HPP

#include <string>
#include <Geode/loader/Event.hpp>

namespace eclipse {
    struct HackingModule {
        std::string_view name;
        enum class State {
            Enabled,
            Tripped
        } state = State::Enabled;
    };
}

namespace eclipse::__internal__ {
    struct VTable {
        // config
        bool(*Config_getBool)(std::string_view, bool const&) = nullptr;
        int(*Config_getInt)(std::string_view, int const&) = nullptr;
        double(*Config_getDouble)(std::string_view, double const&) = nullptr;
        std::string(*Config_getString)(std::string_view, std::string const&) = nullptr;
        std::string_view(*Config_getStringView)(std::string_view, std::string_view const&) = nullptr;
        bool(*Config_getBoolInternal)(std::string_view, bool const&) = nullptr;
        int(*Config_getIntInternal)(std::string_view, int const&) = nullptr;
        double(*Config_getDoubleInternal)(std::string_view, double const&) = nullptr;
        std::string(*Config_getStringInternal)(std::string_view, std::string const&) = nullptr;
        std::string_view(*Config_getStringViewInternal)(std::string_view, std::string_view const&) = nullptr;

        void(*Config_setBool)(std::string_view, bool) = nullptr;
        void(*Config_setInt)(std::string_view, int) = nullptr;
        void(*Config_setDouble)(std::string_view, double) = nullptr;
        void(*Config_setString)(std::string_view, std::string) = nullptr;
        void(*Config_setStringView)(std::string_view, std::string_view) = nullptr;
        void(*Config_setBoolInternal)(std::string_view, bool) = nullptr;
        void(*Config_setIntInternal)(std::string_view, int) = nullptr;
        void(*Config_setDoubleInternal)(std::string_view, double) = nullptr;
        void(*Config_setStringInternal)(std::string_view, std::string) = nullptr;
        void(*Config_setStringViewInternal)(std::string_view, std::string_view) = nullptr;

        // label
        std::string(*FormatRiftString)(std::string_view) = nullptr;
        geode::Result<nullptr_t>(*GetRiftVariableNull)(std::string_view) = nullptr;
        geode::Result<bool>(*GetRiftVariableBool)(std::string_view) = nullptr;
        geode::Result<int64_t>(*GetRiftVariableInt)(std::string_view) = nullptr;
        geode::Result<double>(*GetRiftVariableDouble)(std::string_view) = nullptr;
        geode::Result<std::string>(*GetRiftVariableString)(std::string_view) = nullptr;
        geode::Result<matjson::Value>(*GetRiftVariableObject)(std::string_view) = nullptr;
        void(*SetRiftVariableNull)(std::string, nullptr_t) = nullptr;
        void(*SetRiftVariableBool)(std::string, bool) = nullptr;
        void(*SetRiftVariableInt)(std::string, int64_t) = nullptr;
        void(*SetRiftVariableDouble)(std::string, double) = nullptr;
        void(*SetRiftVariableString)(std::string, std::string) = nullptr;
        void(*SetRiftVariableObject)(std::string, matjson::Value) = nullptr;

        // modules
        void(*RegisterCheat)(std::string, geode::Function<bool()>) = nullptr;
        geode::Result<>(*LoadReplay)(std::filesystem::path const&) = nullptr;
        geode::Result<>(*LoadReplayFromData)(std::span<uint8_t>) = nullptr;
        bool(*CheckCheatsEnabled)() = nullptr;
        bool(*CheckCheatedInAttempt)() = nullptr;
        std::vector<HackingModule>(*GetHackingModules)() = nullptr;

        // std::string(*MenuTab_find)(std::string_view) = nullptr;
        // size_t(*MenuTab_addLabel)(std::string_view, std::string) = nullptr;
    };

    struct FetchVTableEvent : geode::Event<FetchVTableEvent, bool(VTable&)> {
        using Event::Event;
    };

    inline VTable const& getVTable() {
        static VTable vtable{};
        static bool initialized = false;
        if (!initialized) {
            initialized = FetchVTableEvent().send(vtable);
        }
        return vtable;
    }
}

#endif // ECLIPSE_EVENTS_HPP