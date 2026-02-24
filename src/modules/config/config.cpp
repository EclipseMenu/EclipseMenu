#include "config.hpp"

#include <fstream>
#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>

#include <fmt/format.h>

#include <modules/gui/color.hpp>
#include <modules/keybinds/manager.hpp>
#include <modules/labels/setting.hpp>
#include <modules/gui/imgui/animation/easing.hpp>

namespace eclipse::config {

    using CallbackMap = std::unordered_map<std::string_view, std::vector<Function<void()>>>;

    CallbackMap& getCallbacks() {
        static CallbackMap callbacks;
        return callbacks;
    }

    CallbackMap& getTempCallbacks() {
        static CallbackMap tempCallbacks;
        return tempCallbacks;
    }

    matjson::Value& getStorage() {
        static matjson::Value storage = [] {
            auto res = geode::utils::file::readJson(geode::Mod::get()->getSaveDir() / "config.json");
            if (!res) {
                geode::log::warn("Failed to read config file: {}, creating a new one.", res.unwrapErr());
                return matjson::Value::object();
            }

            geode::log::debug("Loaded config file");
            return std::move(res).unwrap();
        }();
        return storage;
    }

    matjson::Value& getTempStorage() {
        static matjson::Value tempStorage;
        return tempStorage;
    }

    /// @brief Load config file from path
    /// @param path Path to the config file
    /// @return True if the file was loaded successfully
    bool loadFile(std::filesystem::path const& path) {
        auto& storage = getStorage();
        auto res = geode::utils::file::readJson(path);
        if (!res) {
            geode::log::warn("Failed to read config file: {}, creating a new one.", res.unwrapErr());
            storage = matjson::Value::object();
            return false;
        }

        geode::log::debug("Loaded config file");
        storage = std::move(res).unwrap();
        return true;
    }

    void executeCallbacks(std::string_view name) {
        auto& callbacks = getCallbacks();
        auto it = callbacks.find(name);
        if (it == callbacks.end()) return;
        for (auto& callback : it->second) {
            callback();
        }
    }

    void executeTempCallbacks(std::string_view name) {
        auto& callbacks = getTempCallbacks();
        auto it = callbacks.find(name);
        if (it == callbacks.end()) return;
        for (auto& callback : it->second) {
            callback();
        }
    }

    void addDelegate(std::string_view key, Function<void()>&& callback, bool first) {
        auto& callbacks = getCallbacks();
        auto& existingCallbacks = callbacks[key];
        first ? void(existingCallbacks.insert(existingCallbacks.begin(), std::move(callback))) :
                void(existingCallbacks.push_back(std::move(callback)));
    }

    void addTempDelegate(std::string_view key, Function<void()>&& callback, bool first) {
        auto& callbacks = getTempCallbacks();
        auto& existingCallbacks = callbacks[key];
        first ? void(existingCallbacks.insert(existingCallbacks.begin(), std::move(callback))) :
                void(existingCallbacks.push_back(std::move(callback)));
    }

    bool hasTemp(std::string_view key) {
        return getTempStorage().contains(key);
    }

    void load() {
        // Unused now
        // auto path = geode::Mod::get()->getSaveDir() / "config.json";
        // if (!loadFile(path)) {
        //     geode::log::warn("Failed to load config file, creating a new one.");
        //     getStorage() = nlohmann::json::object();
        // }
    }

    /// @brief Save config file to path
    /// @param path Path to save the config file
    void saveFile(std::filesystem::path const& path) {
        auto data = getStorage().dump();
        auto res = geode::utils::file::writeStringSafe(path, data);
        if (res.isErr()) {
            geode::log::error("Failed to save config file: {}", res.unwrapErr());
        }
    }

    void save() {
        auto path = geode::Mod::get()->getSaveDir() / "config.json";
        saveFile(path);
    }

    void saveProfile(std::string_view profile) {
        auto profilesDir = geode::Mod::get()->getSaveDir() / "profiles";

        std::error_code ec;
        std::filesystem::create_directories(profilesDir, ec);
        if (ec) {
            geode::log::error("Failed to create profiles directory: {}", ec.message());
            return;
        }

        auto path = profilesDir / fmt::format("{}.json", profile);
        saveFile(path);
    }

    void loadProfile(std::string_view profile) {
        auto profilesDir = geode::Mod::get()->getSaveDir() / "profiles";
        auto path = profilesDir / fmt::format("{}.json", profile);
        if (!loadFile(path)) {
            geode::log::warn("Failed to load profile: {}", profile);
            load(); // Load the default config
        }
    }

    void deleteProfile(std::string_view profile) {
        auto profilesDir = geode::Mod::get()->getSaveDir() / "profiles";
        auto path = profilesDir / fmt::format("{}.json", profile);

        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            std::filesystem::remove(path, ec);
            if (ec) {
                geode::log::error("Failed to delete profile {}: {}", profile, ec.message());
            }
        }
    }

    std::vector<std::string> getProfiles() {
        std::vector<std::string> profiles;
        auto profilesDir = geode::Mod::get()->getSaveDir() / "profiles";
        std::error_code ec;
        if (!std::filesystem::exists(profilesDir, ec)) {
            return profiles;
        }

        if (ec) {
            geode::log::error("Failed to access profiles directory: {}", ec.message());
            return profiles;
        }

        for (auto& entry : std::filesystem::directory_iterator(profilesDir, ec)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                profiles.push_back(geode::utils::string::pathToString(entry.path().stem()));
            }
        }

        if (ec) {
            geode::log::error("Failed to list profiles: {}", ec.message());
        }

        return profiles;
    }

    bool has(std::string_view key) {
        return getStorage().contains(key);
    }

    void erase(std::string_view key) {
        getStorage().erase(key);
    }

    matjson::Type getType(std::string_view key) {
        if (!has(key)) return matjson::Type::Null;
        return getStorage()[key].type();
    }
}
