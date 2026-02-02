#pragma once
#ifndef ECLIPSE_EVENTS_HPP
#define ECLIPSE_EVENTS_HPP

#include <string>
#include <Geode/loader/Event.hpp>

namespace eclipse::__internal__ {
    struct VTable {
        // config
        bool(*Config_getBool)(std::string_view, bool const&) = nullptr;
        int(*Config_getInt)(std::string_view, int const&) = nullptr;
        float(*Config_getFloat)(std::string_view, float const&) = nullptr;
        std::string(*Config_getString)(std::string_view, std::string const&) = nullptr;
        std::string_view(*Config_getStringView)(std::string_view, std::string_view const&) = nullptr;
        bool(*Config_getBoolInternal)(std::string_view, bool const&) = nullptr;
        int(*Config_getIntInternal)(std::string_view, int const&) = nullptr;
        float(*Config_getFloatInternal)(std::string_view, float const&) = nullptr;
        std::string(*Config_getStringInternal)(std::string_view, std::string const&) = nullptr;
        std::string_view(*Config_getStringViewInternal)(std::string_view, std::string_view const&) = nullptr;

        void(*Config_setBool)(std::string_view, bool) = nullptr;
        void(*Config_setInt)(std::string_view, int) = nullptr;
        void(*Config_setFloat)(std::string_view, float) = nullptr;
        void(*Config_setString)(std::string_view, std::string) = nullptr;
        void(*Config_setStringView)(std::string_view, std::string_view) = nullptr;
        void(*Config_setBoolInternal)(std::string_view, bool) = nullptr;
        void(*Config_setIntInternal)(std::string_view, int) = nullptr;
        void(*Config_setFloatInternal)(std::string_view, float) = nullptr;
        void(*Config_setStringInternal)(std::string_view, std::string) = nullptr;
        void(*Config_setStringViewInternal)(std::string_view, std::string_view) = nullptr;

        // std::string(*MenuTab_find)(std::string_view) = nullptr;
        // size_t(*MenuTab_addLabel)(std::string_view, std::string) = nullptr;
    };

    struct FetchVTableEvent : geode::SimpleEvent<FetchVTableEvent, VTable&> {
        using SimpleEvent::SimpleEvent;
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