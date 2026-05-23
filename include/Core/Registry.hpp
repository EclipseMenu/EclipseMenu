#pragma once
#include "Setting.hpp"
#include "../Prelude.hpp"

namespace eclipse {
    struct BoundSetting {
        void* self;
        std::string_view name;
        size_t type;
    };

    namespace __detail {
        template <class T> struct index_for_type {};
        template <class T> constexpr size_t index_for_type_v = index_for_type<T>::value;
    }

    enum class RegistryFindError {
        NotFound,
        TypeMismatch
    };

    constexpr std::string_view format_as(RegistryFindError error) {
        switch (error) {
            case RegistryFindError::NotFound: return "setting not found";
            case RegistryFindError::TypeMismatch: return "setting type mismatch";
            default: return "unknown error";
        }
    }

    class ECLIPSE_DLL Registry {
    public:
        Registry(Registry const&) = delete;
        Registry(Registry&&) = delete;
        Registry& operator=(Registry const&) = delete;
        Registry& operator=(Registry&&) = delete;

        static Registry* get() ECLIPSE_EVENT_METHOD(Registry::get);

        void bindSetting(BoundSetting const& setting) ECLIPSE_EVENT_METHOD(Registry::bindSetting, this, setting);
        BoundSetting* findSetting(std::string_view name) ECLIPSE_EVENT_METHOD(Registry::findSetting, this, name);

        Result<> loadFromFile(std::filesystem::path const& path) ECLIPSE_EVENT_METHOD(Registry::loadFromFile, this, path);
        Result<> saveToFile(std::filesystem::path const& path) ECLIPSE_EVENT_METHOD(Registry::saveToFile, this, path);

        template <class T>
        Result<Setting<T>*, RegistryFindError> find(std::string_view name) {
            auto setting = this->findSetting(name);
            if (!setting) {
                return Err(RegistryFindError::NotFound);
            }

            if (setting->type != __detail::index_for_type_v<T>) {
                return Err(RegistryFindError::TypeMismatch);
            }

            return Ok(static_cast<Setting<T>*>(setting->self));
        }

        template <class T>
        Result<T const&, RegistryFindError> get(std::string_view name) {
            GEODE_UNWRAP_INTO(auto* setting, this->find<T>(name));
            return Ok(setting->value());
        }

        template <class T>
        Result<void, RegistryFindError> set(std::string_view name, T&& value) {
            GEODE_UNWRAP_INTO(auto* setting, this->find<T>(name));
            setting->set(std::forward<T>(value));
            return Ok();
        }

    private:
        Registry();
        ~Registry();

    private:
        std::vector<BoundSetting> m_settings;
        StringMap<size_t> m_settingsMap;
        matjson::Value m_json;
    };
}