#include <Core/Registry.hpp>
#include <Core/Config.hpp>

namespace eclipse {
    Registry* Registry::get() {
        static Registry instance;
        return &instance;
    }

    static bool loadSetting(BoundSetting const& setting, matjson::Value const& value, bool notify = true) {
        using namespace __detail;
        if (setting.type >= type_table.size()) return false;
        auto& ops = type_table[setting.type];
        return ops.deserialize(value, setting.self, notify);
    }

    static matjson::Value saveSetting(BoundSetting const& setting) {
        using namespace __detail;
        if (setting.type >= type_table.size()) return matjson::Value();
        auto& ops = type_table[setting.type];
        return ops.serialize(setting.self);
    }

    void Registry::bindSetting(BoundSetting const& setting) {
        m_settings.push_back(setting);
        m_settingsMap[std::string(setting.name)] = m_settings.size() - 1;
        loadSetting(setting, m_json[setting.name], false);
    }

    BoundSetting* Registry::findSetting(std::string_view name) {
        auto it = m_settingsMap.find(name);
        return it != m_settingsMap.end() ? &m_settings[it->second] : nullptr;
    }

    Result<> Registry::loadFromFile(std::filesystem::path const& path) {
        GEODE_UNWRAP_INTO(m_json, geode::utils::file::readJson(path));

        for (auto const& setting : m_settings) {
            if (m_json.contains(setting.name)) {
                if (!loadSetting(setting, m_json[setting.name])) {
                    log::error("Failed to load setting \"{}\" from config file", setting.name);
                }
            }
        }

        return Ok();
    }

    Result<> Registry::saveToFile(std::filesystem::path const& path) {
        for (auto const& setting : m_settings) {
            m_json[setting.name] = saveSetting(setting);
        }

        auto str = m_json.dump(4);
        return geode::utils::file::writeStringSafe(path, str);
    }

    Registry::Registry() {
        auto path = geode::Mod::get()->getSaveDir() / "config.json";
        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            auto res = this->loadFromFile(path);
            if (!res) {
                log::error("Failed to load config: {}", res.unwrapErr());
            }
        }
    }

    Registry::~Registry() = default;

    $on_mod(DataSaved) {
        auto path = geode::Mod::get()->getSaveDir() / "config.json";
        auto res = Registry::get()->saveToFile(path);
        if (!res) {
            log::error("Failed to save config: {}", res.unwrapErr());
        }
    }
}
