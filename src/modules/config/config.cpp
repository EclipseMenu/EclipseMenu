#include "config.hpp"

#include <fstream>
#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>

#include <fmt/format.h>
#include <nlohmann/json.hpp>

#include <modules/gui/color.hpp>
#include <modules/keybinds/manager.hpp>
#include <modules/labels/setting.hpp>
#include <modules/gui/imgui/animation/easing.hpp>

namespace eclipse::config {

    using CallbackMap = std::unordered_map<std::string_view, std::vector<std::function<void()>>>;

    CallbackMap& getCallbacks() {
        static CallbackMap callbacks;
        return callbacks;
    }

    CallbackMap& getTempCallbacks() {
        static CallbackMap tempCallbacks;
        return tempCallbacks;
    }

    nlohmann::json& getStorage() {
        static nlohmann::json storage = [] {
            auto path = geode::Mod::get()->getSaveDir() / "config.json";
            std::ifstream file(path);
            if (!file.is_open()) {
                geode::log::warn("Failed to open config file, creating a new one.");
                return nlohmann::json::object();
            }
            auto res = nlohmann::json::parse(file, nullptr, false);
            if (res.is_discarded()) {
                geode::log::warn("Failed to parse config file, creating a new one.");
                return nlohmann::json::object();
            }
            geode::log::debug("Loaded config file");
            return res;
        }();
        return storage;
    }

    nlohmann::json& getTempStorage() {
        static nlohmann::json tempStorage;
        return tempStorage;
    }

    /// @brief Load config file from path
    /// @param path Path to the config file
    /// @return True if the file was loaded successfully
    bool loadFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        auto& storage = getStorage();
        storage = nlohmann::json::parse(file, nullptr, false);
        file.close();

        return !storage.is_discarded();
    }

    void executeCallbacks(std::string_view name) {
        auto& callbacks = getCallbacks();
        auto it = callbacks.find(name);
        if (it == callbacks.end()) return;
        for (const auto& callback : it->second) {
            callback();
        }
    }

    void executeTempCallbacks(std::string_view name) {
        auto& callbacks = getTempCallbacks();
        auto it = callbacks.find(name);
        if (it == callbacks.end()) return;
        for (const auto& callback : it->second) {
            callback();
        }
    }

    void addDelegate(std::string_view key, std::function<void()> callback) {
        auto& callbacks = getCallbacks();
        callbacks[key].push_back(std::move(callback));
    }

    void addTempDelegate(std::string_view key, std::function<void()> callback) {
        auto& callbacks = getTempCallbacks();
        callbacks[key].push_back(std::move(callback));
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
    void saveFile(const std::filesystem::path& path) {
        std::ofstream file(path);
        file << getStorage().dump(4, ' ', false, nlohmann::detail::error_handler_t::ignore);
        file.close();
    }

    void save() {
        auto path = geode::Mod::get()->getSaveDir() / "config.json";
        saveFile(path);
    }

    void saveProfile(std::string_view profile) {
        auto profilesDir = geode::Mod::get()->getSaveDir() / "profiles";
        std::filesystem::create_directories(profilesDir);

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

        if (std::filesystem::exists(path))
            std::filesystem::remove(path);
    }

    std::vector<std::string> getProfiles() {
        std::vector<std::string> profiles;
        auto profilesDir = geode::Mod::get()->getSaveDir() / "profiles";
        if (!std::filesystem::exists(profilesDir)) {
            return profiles;
        }

        for (auto& entry : std::filesystem::directory_iterator(profilesDir)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                profiles.push_back(entry.path().stem().string());
            }
        }

        return profiles;
    }

    bool has(std::string_view key) {
        return getStorage().contains(key);
    }

    nlohmann::detail::value_t getType(std::string_view key) {
        if (!has(key)) return nlohmann::detail::value_t::null;
        return getStorage().at(key).type();
    }

    template <typename T>
    T get(std::string_view key, const T& defaultValue)  {
        if (!has(key))
            return defaultValue;

        return getStorage().at(key).get<T>();
    }

    template <typename T>
    geode::Result<T> get(std::string_view key) {
        if (!has(key))
            return geode::Err(fmt::format("Key '{}' does not exist", key));

        return geode::Ok(getStorage().at(key).get<T>());
    }

    template <typename T>
    void set(std::string_view key, const T& value) {
        getStorage()[key] = value;
        executeCallbacks(key);
    }

    template <typename T>
    bool is(std::string_view key) {
        if (!has(key))
            return false;

        auto type = getType(key);
        if constexpr (std::is_same_v<T, std::string>) {
            return type == nlohmann::detail::value_t::string;
        } else if constexpr (std::is_same_v<T, bool>) {
            return type == nlohmann::detail::value_t::boolean;
        } else if constexpr (std::is_same_v<T, int>) {
            return type == nlohmann::detail::value_t::number_integer;
        } else if constexpr (std::is_same_v<T, float>) {
            return type == nlohmann::detail::value_t::number_float;
        }

        return false;
    }

    template <typename T>
    void setIfEmpty(std::string_view key, const T& value) {
        if (!has(key))
            set(key, value);
    }

    template <typename T>
    T getTemp(std::string_view key, const T& defaultValue) {
        if (!hasTemp(key))
            return defaultValue;

        return getTempStorage().at(key).get<T>();
    }

    template <typename T>
    geode::Result<T> getTemp(std::string_view key) {
        if (!hasTemp(key))
            return geode::Err(fmt::format("Key '{}' does not exist", key));

        return geode::Ok(getTempStorage().at(key).get<T>());
    }

    template <typename T>
    void setTemp(std::string_view key, const T& value) {
        getTempStorage()[key] = value;
        executeTempCallbacks(key);
    }

    // Explicit instantiations
    #define INSTANTIATE_IMPL_VOID(type, def) \
        template void set<type>(std::string_view, def); \
        template void setIfEmpty<type>(std::string_view, def); \
        template void setTemp<type>(std::string_view, def)

    #define INSTANTIATE_IMPL_NON_VOID(type, def) \
        template type get<type>(std::string_view, def); \
        template geode::Result<type> get<type>(std::string_view); \
        template bool is<type>(std::string_view); \
        template type getTemp<type>(std::string_view, def); \
        template geode::Result<type> getTemp<type>(std::string_view);

    #define INSTANTIATE_IMPL(type, def) \
        INSTANTIATE_IMPL_VOID(type, def); \
        INSTANTIATE_IMPL_NON_VOID(type, def)

    #define INSTANTIATE(type) INSTANTIATE_IMPL(type, const type&)

    INSTANTIATE(std::filesystem::path);
    INSTANTIATE(std::string);
    INSTANTIATE(std::string_view);
    INSTANTIATE_IMPL_VOID(char[1], char const (&)[1]);
    INSTANTIATE(int);
    INSTANTIATE(int64_t);
    INSTANTIATE(uint64_t);
    #if defined(GEODE_IS_MACOS) || defined(GEODE_IS_ANDROID32)
    INSTANTIATE(size_t);
    #endif
    INSTANTIATE(float);
    INSTANTIATE(double);
    INSTANTIATE(bool);
    INSTANTIATE(gui::Color);
    INSTANTIATE(keybinds::Keys);
    INSTANTIATE(std::vector<labels::LabelSettings>);
    INSTANTIATE(std::vector<nlohmann::json>);
    INSTANTIATE(gui::animation::Easing);
    INSTANTIATE(gui::animation::EasingMode);
}
