#pragma once

#ifdef ECLIPSE_USING_LINK_API
    #warning "Already using linked API, either remove \"Eclipse.hpp\" include to use Event API, or don't include \"Events.hpp\""
#else
    #ifndef ECLIPSE_USING_EVENT_API
        #define ECLIPSE_USING_EVENT_API
    #endif
#endif

#ifdef ECLIPSE_EVENT_METHOD
    #undef ECLIPSE_EVENT_METHOD
#endif

#include <filesystem>
#include <string_view>
#include <Geode/Result.hpp>

namespace eclipse {
    struct BoundSetting;
    class Registry;
    class InputManager;

    namespace __detail::event {
        constexpr size_t API_VERSION = 2;

        struct Table {
            size_t version = API_VERSION;

            // == Registry == //
            Registry*(*Registry_get)();
            void(Registry::*Registry_bindSetting)(BoundSetting const&);
            BoundSetting*(Registry::*Registry_findSetting)(std::string_view);
            geode::Result<>(Registry::*Registry_loadFromFile)(std::filesystem::path const&);
            geode::Result<>(Registry::*Registry_saveToFile)(std::filesystem::path const&);

            // == InputManager == //
            InputManager*(*InputManager_get)();
        };

        struct FetchTableEvent : geode::Event<FetchTableEvent, bool(Table*&)> {
            using Event::Event;
        };

        inline Table* getTable() {
            static Table* table = nullptr;
            if (!table) FetchTableEvent().send(table);
            return table;
        }

        #define ECLIPSE_EVENT_SINGLETON_GET(Class) \
            inline eclipse::Class* get() { \
                static eclipse::Class* instance = nullptr; \
                if (!instance) { \
                    if (auto table = getTable()) { \
                        instance = table->Class##_get(); \
                    } \
                } \
                return instance; \
            }

        #define ECLIPSE_EVENT_METHOD_VOID(cls, name, args, ...) \
            inline void name args { \
                if (!self) return; \
                if (auto table = getTable()) { \
                    (self->*table->cls##_##name)(__VA_ARGS__); \
                } \
            }

        #define ECLIPSE_EVENT_METHOD_PTR(cls, name, type, args, ...) \
            inline type name args { \
                if (!self) return nullptr; \
                if (auto table = getTable()) { \
                    return (self->*table->cls##_##name)(__VA_ARGS__); \
                } \
                return nullptr; \
            }

        #define ECLIPSE_EVENT_METHOD_RESULT(cls, name, type, args, ...) \
            inline geode::Result<type> name args { \
                if (!self) return geode::Err("Instance not available"); \
                if (auto table = getTable()) { \
                    return (self->*table->cls##_##name)(__VA_ARGS__); \
                } \
                return geode::Err("Event API not available"); \
            }

        namespace Registry {
            ECLIPSE_EVENT_SINGLETON_GET(Registry)

            ECLIPSE_EVENT_METHOD_VOID(Registry, bindSetting, (eclipse::Registry* self, BoundSetting const& setting), setting)
            ECLIPSE_EVENT_METHOD_PTR(Registry, findSetting, BoundSetting*, (eclipse::Registry* self, std::string_view name), name)
            ECLIPSE_EVENT_METHOD_RESULT(Registry, loadFromFile,, (eclipse::Registry* self, std::filesystem::path const& path), path)
            ECLIPSE_EVENT_METHOD_RESULT(Registry, saveToFile,, (eclipse::Registry* self, std::filesystem::path const& path), path)
        }

        namespace InputManager {
            ECLIPSE_EVENT_SINGLETON_GET(InputManager)
        }

        #undef ECLIPSE_EVENT_SINGLETON_GET
        #undef ECLIPSE_EVENT_METHOD_VOID
        #undef ECLIPSE_EVENT_METHOD_PTR
        #undef ECLIPSE_EVENT_METHOD_RESULT
    }
}