#pragma once
#include <Utils/FixedString.hpp>

#include "Registry.hpp"
#include "Setting.hpp"

namespace eclipse {
    #define ECLIPSE_CONFIG_TYPES(X) \
        X(bool, 0) \
        X(int64_t, 1) \
        X(double, 2) \
        X(std::string, 3)

    namespace __detail {
        struct TypeOps {
            using SerializeFn = matjson::Value(*)(void const* value);
            using DeserializeFn = bool(*)(matjson::Value const& json, void* out, bool notify);

            SerializeFn serialize;
            DeserializeFn deserialize;
        };

        template <class T>
        matjson::Value serialize_type(void const* value) {
            return matjson::Value(*static_cast<T const*>(value));
        }

        template <class T>
        bool deserialize_type(matjson::Value const& json, void* out, bool notify) {
            auto res = json.as<T>();
            if (!res) return false;
            Setting<T>* setting = static_cast<Setting<T>*>(out);
            if (notify) setting->set(std::move(*res));
            else setting->value() = std::move(*res);
            return true;
        }

        #define ECLIPSE_TYPE_OPS(type, index) { &serialize_type<type>, &deserialize_type<type> },
        #define ECLIPSE_DECL_INDEX(type, index) \
            template <> \
            struct index_for_type<type> { \
                static constexpr size_t value = index; \
            };

        inline constexpr auto type_table = std::to_array<TypeOps>({
            ECLIPSE_CONFIG_TYPES(ECLIPSE_TYPE_OPS)
        });

        ECLIPSE_CONFIG_TYPES(ECLIPSE_DECL_INDEX)

        #undef ECLIPSE_DECL_INDEX
        #undef ECLIPSE_TYPE_OPS
    }

    template <class T>
    T default_value() {
        if constexpr (std::is_default_constructible_v<T>) {
            return T{};
        } else {
            static_assert(sizeof(T) == 0, "Type must be default constructible or have a specialization of default_value");
            std::unreachable();
        }
    }

    template <class T, FixedString Name>
    T default_value() { return default_value<T>(); }

    template <class T, FixedString Name>
    class ConfigSetting : public Setting<T> {
    public:
        using Setting<T>::Setting;

        static ConfigSetting& get() {
            static ConfigSetting instance;
            return instance;
        }

        constexpr static std::string_view name() noexcept { return Name; }

    private:
        ConfigSetting() : Setting<T>(default_value<T, Name>()) {
            Registry::get()->bindSetting({
                .self = this,
                .name = Name,
                .type = __detail::index_for_type_v<T>
            });
        }
    };

    namespace config {
        template <class T, FixedString Name>
        [[nodiscard]] ConfigSetting<T, Name>& ref() {
            return ConfigSetting<T, Name>::get();
        }

        template <class T, FixedString Name>
        [[nodiscard]] T const& get() {
            return ref<T, Name>().value();
        }

        template <class T, FixedString Name>
        void set(T&& value) {
            ref<T, Name>().set(std::forward<T>(value));
        }

        template <class T, FixedString Name>
        void set(T const& value) {
            ref<T, Name>().set(value);
        }

        template <class T, FixedString Name>
        size_t listen(typename ConfigSetting<T, Name>::ObserverFn fn) {
            return ref<T, Name>().listen(std::move(fn));
        }

        template <class T, FixedString Name>
        void unlisten(size_t id) {
            ref<T, Name>().unlisten(id);
        }

        template <class T>
        Result<T const&, RegistryFindError> get(std::string_view name) {
            return Registry::get()->get<T>(name);
        }

        template <class T>
        Result<void, RegistryFindError> set(std::string_view name, T&& value) {
            return Registry::get()->set<T>(name, std::forward<T>(value));
        }

        template <class T>
        Result<void, RegistryFindError> set(std::string_view name, T const& value) {
            return Registry::get()->set<T>(name, value);
        }
    }
}