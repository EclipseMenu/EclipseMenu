#include "config.hpp"

#include <fstream>

namespace eclipse::config {

    static nlohmann::json storage;
    static nlohmann::json tempStorage;

    nlohmann::json& getStorage() {
        return storage;
    }

    nlohmann::json& getTempStorage() {
        return tempStorage;
    }

    /// @brief Load config file from path
    /// @param path Path to the config file
    /// @return True if the file was loaded successfully
    bool loadFile(const std::filesystem::path& path) {
        std::ifstream file(path);
        if (!file.is_open()) return false;

        storage = nlohmann::json::parse(file, nullptr, false);
        file.close();

        return !getStorage().is_discarded();
    }

    void load() {
        auto path = geode::Mod::get()->getSaveDir() / "config.json";
        if (!loadFile(path)) {
            geode::log::warn("Failed to load config file, creating a new one.");
            storage = nlohmann::json::object();
        }
    }

    /// @brief Save config file to path
    /// @param path Path to save the config file
    void saveFile(const std::filesystem::path& path) {
        std::ofstream file(path);
        file << getStorage().dump(4);
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

}