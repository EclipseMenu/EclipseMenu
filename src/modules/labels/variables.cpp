#include "variables.hpp"
#include <modules/config/config.hpp>
#include <utils.hpp>

#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/Loader.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <rift/config.hpp>

namespace eclipse::labels {

    rift::Value getConfigValue(std::span<rift::Value> args) {
        if (args.size() != 1)
            return rift::Value::from("<cfg requires string argument>");
        auto key = args[0].toString();
        if (!config::has(key))
            return rift::Value::null();
        switch (config::getType(key)) {
            case nlohmann::detail::value_t::string:
                return rift::Value::string(config::get<std::string>(key));
            case nlohmann::detail::value_t::boolean:
                return rift::Value::boolean(config::get<bool>(key));
            case nlohmann::detail::value_t::number_integer:
                return rift::Value::integer(config::get<int>(key));
            case nlohmann::detail::value_t::number_float:
                return rift::Value::floating(config::get<float>(key));
            default:
                return rift::Value::null();
        }
    }

    $on_mod(Loaded) {
        rift::config::addRuntimeFunction("cfg", getConfigValue);
    }

    VariableManager& VariableManager::get() {
        static VariableManager instance;
        static bool initialized = false;
        if (!initialized) {
            instance.init();
            initialized = true;
        }
        return instance;
    }

    void VariableManager::init() {
        auto* mod = geode::Mod::get();
        auto* loader = geode::Loader::get();

        // Mod variables
        m_variables["modVersion"] = rift::Value::string(mod->getVersion().toNonVString());
        m_variables["geodeVersion"] = rift::Value::string(geode::Loader::get()->getVersion().toNonVString());
        m_variables["platform"] = rift::Value::string(
            GEODE_WINDOWS("Windows")
            GEODE_ANDROID("Android")
            GEODE_MACOS("macOS")
            GEODE_IOS("iOS")
        );
        m_variables["gameVersion"] = rift::Value::string(loader->getGameVersion());
        auto allMods = loader->getAllMods();
        m_variables["totalMods"] = rift::Value::integer(static_cast<int>(allMods.size()));
        m_variables["enabledMods"] = rift::Value::integer(static_cast<int>(std::ranges::count_if(allMods, [](auto* mod) {
            return mod->shouldLoad();
        })));

        // Emojis :D
        m_variables["starEmoji"] = rift::Value::string("⭐");
        m_variables["moonEmoji"] = rift::Value::string("🌙");
        m_variables["heartEmoji"] = rift::Value::string("❤️");
        m_variables["checkEmoji"] = rift::Value::string("✅");
        m_variables["crossEmoji"] = rift::Value::string("❌");
        m_variables["exclamationEmoji"] = rift::Value::string("❗");
        m_variables["questionEmoji"] = rift::Value::string("❓");
        m_variables["fireEmoji"] = rift::Value::string("🔥");
        m_variables["snowflakeEmoji"] = rift::Value::string("❄️");
        m_variables["catEmoji"] = rift::Value::string("🐱");
        m_variables["dogEmoji"] = rift::Value::string("🐶");
        m_variables["speakingHeadEmoji"] = rift::Value::string("🗣️");
        m_variables["robotEmoji"] = rift::Value::string("🤖");
        m_variables["alienEmoji"] = rift::Value::string("👽");
        m_variables["ghostEmoji"] = rift::Value::string("👻");
        m_variables["skullEmoji"] = rift::Value::string("💀");
        m_variables["babyEmoji"] = rift::Value::string("👶");

        // Fetch everything else
        m_variables["fps"] = rift::Value::floating(0.f);
        m_variables["realFps"] = rift::Value::floating(0.f);
        refetch();
    }

    void VariableManager::setVariable(const std::string& name, const rift::Value& value) {
        m_variables[name] = value;
    }

    rift::Value VariableManager::getVariable(const std::string& name) const {
        auto it = m_variables.find(name);
        if (it == m_variables.end()) return rift::Value::null();
        return it->second;
    }

    bool VariableManager::hasVariable(const std::string& name) const {
        return m_variables.contains(name);
    }

    void VariableManager::removeVariable(const std::string& name) {
        m_variables.erase(name);
    }

    enum class LevelDifficulty {
        NA, Auto, Easy,
        Normal, Hard, Harder,
        Insane, EasyDemon, MediumDemon,
        HardDemon, InsaneDemon, ExtremeDemon
    };

    static LevelDifficulty getLevelDifficulty(GJGameLevel* level) {
        if (level->m_autoLevel) return LevelDifficulty::Auto;
        auto diff = level->m_difficulty;

        if (level->m_ratingsSum != 0)
            diff = static_cast<GJDifficulty>(level->m_ratingsSum / 10);

        if (level->m_demon > 0) {
            switch (level->m_demonDifficulty) {
                case 3: return LevelDifficulty::EasyDemon;
                case 4: return LevelDifficulty::MediumDemon;
                case 5: return LevelDifficulty::InsaneDemon;
                case 6: return LevelDifficulty::ExtremeDemon;
                default: return LevelDifficulty::HardDemon;
            }
        }

        switch (diff) {
            case GJDifficulty::Easy: return LevelDifficulty::Easy;
            case GJDifficulty::Normal: return LevelDifficulty::Normal;
            case GJDifficulty::Hard: return LevelDifficulty::Hard;
            case GJDifficulty::Harder: return LevelDifficulty::Harder;
            case GJDifficulty::Insane: return LevelDifficulty::Insane;
            case GJDifficulty::Demon: return LevelDifficulty::HardDemon;
            default: return LevelDifficulty::NA;
        }
    }

    const char* getLevelDifficultyString(LevelDifficulty diff) {
        switch (diff) {
            case LevelDifficulty::NA: return "N/A";
            case LevelDifficulty::Auto: return "Auto";
            case LevelDifficulty::Easy: return "Easy";
            case LevelDifficulty::Normal: return "Normal";
            case LevelDifficulty::Hard: return "Hard";
            case LevelDifficulty::Harder: return "Harder";
            case LevelDifficulty::Insane: return "Insane";
            case LevelDifficulty::EasyDemon: return "Easy Demon";
            case LevelDifficulty::MediumDemon: return "Medium Demon";
            case LevelDifficulty::HardDemon: return "Hard Demon";
            case LevelDifficulty::InsaneDemon: return "Insane Demon";
            case LevelDifficulty::ExtremeDemon: return "Extreme Demon";
            default: return "Unknown";
        }
    }

    const char* getLevelDifficultyKey(LevelDifficulty diff) {
        switch (diff) {
            case LevelDifficulty::NA: return "na";
            case LevelDifficulty::Auto: return "auto";
            case LevelDifficulty::Easy: return "easy";
            case LevelDifficulty::Normal: return "normal";
            case LevelDifficulty::Hard: return "hard";
            case LevelDifficulty::Harder: return "harder";
            case LevelDifficulty::Insane: return "insane";
            case LevelDifficulty::EasyDemon: return "easy_demon";
            case LevelDifficulty::MediumDemon: return "medium_demon";
            case LevelDifficulty::HardDemon: return "hard_demon";
            case LevelDifficulty::InsaneDemon: return "insane_demon";
            case LevelDifficulty::ExtremeDemon: return "extreme_demon";
            default: return "unknown";
        }
    }

    static std::string formatTime(int millis) {
        if (millis == 0) return "N/A";
        double seconds = millis / 1000.0;
        return utils::formatTime(seconds);
    }

    static double getFPS() {
        static std::chrono::time_point<std::chrono::system_clock> s_lastUpdate;
        auto now = std::chrono::system_clock::now();
        auto duration = now - s_lastUpdate;
        s_lastUpdate = now;
        double micros = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(duration).count());
        return 1'000'000.0 / micros;
    }

    static double accumulateFPS(double fps) {
        static float s_currentFps = 0.f;
        static float s_fpsAccumDelta = 0.f;
        static uint32_t s_frames = 0;
        s_fpsAccumDelta += 1.f / fps;
        s_frames++;
        if (s_fpsAccumDelta >= 0.1f) {
            s_currentFps = s_frames / s_fpsAccumDelta;
            s_frames = 0;
            s_fpsAccumDelta = 0.f;
        }
        return s_currentFps;
    }

    void VariableManager::updateFPS() {
        auto fps = getFPS();
        m_variables["realFps"] = rift::Value::floating(fps);
        m_variables["fps"] = rift::Value::floating(accumulateFPS(fps));
    }

    void VariableManager::fetchGeneralData() {
        auto* gameManager = GameManager::get();
        m_variables["username"] = rift::Value::string(gameManager->m_playerName);
        m_variables["cubeIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Cube));
        m_variables["shipIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Ship));
        m_variables["ballIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Ball));
        m_variables["ufoIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::UFO));
        m_variables["waveIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Wave));
        m_variables["robotIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Robot));
        m_variables["spiderIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Spider));
        m_variables["swingIcon"] = rift::Value::integer(utils::getPlayerIcon(PlayerMode::Swing));
    }

    void VariableManager::fetchTimeData() {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto localTime = std::localtime(&time);
        m_variables["hour"] = rift::Value::integer(localTime->tm_hour);
        m_variables["minute"] = rift::Value::integer(localTime->tm_min);
        m_variables["second"] = rift::Value::integer(localTime->tm_sec);
        m_variables["day"] = rift::Value::integer(localTime->tm_mday);
        m_variables["month"] = rift::Value::integer(localTime->tm_mon + 1);
        m_variables["monthName"] = rift::Value::string(utils::getMonthName(localTime->tm_mon));
        m_variables["year"] = rift::Value::integer(localTime->tm_year + 1900);
        m_variables["clock"] = rift::Value::string(utils::getClock());
        m_variables["clock12"] = rift::Value::string(utils::getClock(true));
    }

    void VariableManager::fetchHacksData() {
        m_variables["isCheating"] = rift::Value::boolean(config::getTemp("hasCheats", false));
        m_variables["noclip"] = rift::Value::boolean(config::get("player.noclip", false));
        m_variables["speedhack"] = rift::Value::boolean(config::get("global.speedhack.toggle", false));
        m_variables["speedhackSpeed"] = rift::Value::floating(config::get("global.speedhack", 1.f));
        m_variables["framestepper"] = rift::Value::boolean(config::get("player.framestepper", false));
    }

    static std::string const& cachedBase64Decode(const std::string& str) {
        static std::string s_lastStr;
        static std::string s_lastDecoded;
        if (str == s_lastStr) return s_lastDecoded;
        s_lastStr = str;
        if (str.empty()) return s_lastDecoded = "";
        s_lastDecoded = cocos2d::ZipUtils::base64URLDecode(str);
        return s_lastDecoded;
    }

    void VariableManager::fetchLevelData(GJGameLevel* level) {
        if (!level) {
            // Reset all level variables
            constexpr std::array keys = {
                "levelID", "levelName", "levelDescription", "author",
                "isRobtopLevel", "levelAttempts", "levelStars",
                "difficulty", "difficultyKey", "practicePercent",
                "bestPercent", "bestTime", "best"
            };
            for (const auto& key : keys) {
                removeVariable(key);
            }
            return;
        }

        auto levelID = level->m_levelID;
        bool isRobtopLevel = (levelID > 0 && levelID < 100) || (levelID >= 3001 && levelID <= 6000);
        auto levelDifficulty = getLevelDifficulty(level);
        m_variables["levelID"] = rift::Value::integer(levelID);
        m_variables["levelName"] = rift::Value::string(level->m_levelName);
        m_variables["levelDescription"] = rift::Value::string(cachedBase64Decode(level->m_levelDesc));
        m_variables["author"] = rift::Value::string(isRobtopLevel ? "RobTop" : level->m_creatorName);
        m_variables["isRobtopLevel"] = rift::Value::boolean(isRobtopLevel);
        m_variables["levelAttempts"] = rift::Value::integer(level->m_attempts);
        m_variables["levelStars"] = rift::Value::integer(level->m_stars);
        m_variables["difficulty"] = rift::Value::string(getLevelDifficultyString(levelDifficulty));
        m_variables["difficultyKey"] = rift::Value::string(getLevelDifficultyKey(levelDifficulty));
        m_variables["practicePercent"] = rift::Value::integer(level->m_practicePercent);
        m_variables["bestPercent"] = rift::Value::integer(level->m_normalPercent);
        m_variables["bestTime"] = rift::Value::floating(level->m_bestTime);
        if (GJBaseGameLayer::get()->m_isPlatformer)
            m_variables["best"] = rift::Value::string(formatTime(level->m_bestTime));
        else
            m_variables["best"] = rift::Value::integer(level->m_normalPercent);
    }

    void VariableManager::fetchPlayerData(PlayerObject* player, bool isPlayer2) {
        if (!player) {
            // Reset all player variables
            constexpr std::array keys = {
                "playerX", "playerY", "player2X", "player2Y"
            };
            for (const auto& key : keys) {
                removeVariable(key);
            }
            if (!isPlayer2) {
                auto gamemode = utils::getGameMode(nullptr);
                m_variables["gamemode"] = rift::Value::string(utils::gameModeName(gamemode));
                m_variables["playerIcon"] = rift::Value::integer(utils::getPlayerIcon(gamemode));
            }
            return;
        }

        m_variables[isPlayer2 ? "player2X" : "playerX"] = rift::Value::floating(player->m_position.x);
        m_variables[isPlayer2 ? "player2Y" : "playerY"] = rift::Value::floating(player->m_position.y);

        if (!isPlayer2) {
            auto gamemode = utils::getGameMode(player);
            m_variables["gamemode"] = rift::Value::string(utils::gameModeName(gamemode));
            m_variables["playerIcon"] = rift::Value::integer(utils::getPlayerIcon(gamemode));
        }
    }

    void VariableManager::fetchGameplayData(GJBaseGameLayer* gameLayer) {
        if (!gameLayer) {
            // Reset all gameplay variables
            constexpr std::array keys = {
                "attempt", "isTestMode", "isPracticeMode", "isPlatformer",
                "levelTime", "levelLength", "levelDuration", "time", "frame",
                "isDead", "isDualMode", "noclipDeaths", "noclipAccuracy", "progress",
                "editorMode", "realProgress", "objects"
            };
            for (const auto& key : keys) {
                removeVariable(key);
            }

            // Reset level and player data
            fetchLevelData(nullptr);
            fetchPlayerData(nullptr, false);
            fetchPlayerData(nullptr, true);
            return;
        }

        m_variables["attempt"] = rift::Value::integer(gameLayer->m_attempts);
        m_variables["isTestMode"] = rift::Value::boolean(gameLayer->m_isTestMode);
        m_variables["isPracticeMode"] = rift::Value::boolean(gameLayer->m_isPracticeMode);
        m_variables["isPlatformer"] = rift::Value::boolean(gameLayer->m_isPlatformer);
        m_variables["levelTime"] = rift::Value::floating(gameLayer->m_gameState.m_levelTime);
        m_variables["levelLength"] = rift::Value::floating(gameLayer->m_levelLength);
        m_variables["levelDuration"] = rift::Value::floating(gameLayer->m_level->m_timestamp / 240.f);
        m_variables["time"] = rift::Value::string(utils::formatTime(gameLayer->m_gameState.m_levelTime));
        m_variables["frame"] = rift::Value::integer(gameLayer->m_gameState.m_levelTime * 240.0);
        m_variables["isDead"] = rift::Value::boolean(gameLayer->m_player1->m_isDead);
        m_variables["isDualMode"] = rift::Value::boolean(gameLayer->m_player2 != nullptr && gameLayer->m_player2->isRunning()); // can m_isDualMode be added already
        m_variables["noclipDeaths"] = rift::Value::integer(config::getTemp("noclipDeaths", 0));
        m_variables["noclipAccuracy"] = rift::Value::floating(config::getTemp("noclipAccuracy", 100.f));
        m_variables["progress"] = rift::Value::floating(utils::getActualProgress(gameLayer));

        if (auto* pl = PlayLayer::get()) {
            m_variables["editorMode"] = rift::Value::boolean(false);
            m_variables["realProgress"] = rift::Value::floating(pl->getCurrentPercent());
            m_variables["objects"] = rift::Value::integer(gameLayer->m_level->m_objectCount);
        } else if (auto* ed = LevelEditorLayer::get()) {
            m_variables["editorMode"] = rift::Value::boolean(true);
            m_variables["realProgress"] = rift::Value::floating(0.f);
            m_variables["objects"] = rift::Value::integer(ed->m_objects->count());
        }

        fetchLevelData(gameLayer->m_level);
        fetchPlayerData(gameLayer->m_player1, false);
        fetchPlayerData(gameLayer->m_player2, true);
    }

    void VariableManager::refetch() {
        // Game variables
        fetchGeneralData();

        // Hack states (only the important ones
        fetchHacksData();

        // Time
        fetchTimeData();

        // Game state
        fetchGameplayData(GJBaseGameLayer::get());
    }

    class $modify(BestRunPLHook, PlayLayer) {
        struct Fields {
            float m_runFrom = 0.f;
            float m_lastRunFrom = 0.f;
            float m_bestRun = 0.f;
            float m_lastBestRun = 0.f;
        };

        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

            auto& manager = VariableManager::get();
            manager.setVariable("runFrom", rift::Value::floating(0.f));
            manager.setVariable("bestRun", rift::Value::floating(0.f));

            return true;
        }

        void saveBestRun() {
            auto fields = m_fields.self();
            fields->m_bestRun = utils::getActualProgress(this);
            if ((fields->m_bestRun - fields->m_runFrom) >= (fields->m_lastBestRun - fields->m_lastRunFrom)) {
                fields->m_lastBestRun = fields->m_bestRun;
                fields->m_lastRunFrom = fields->m_runFrom;
                auto& manager = VariableManager::get();
                manager.setVariable("runFrom", rift::Value::floating(fields->m_runFrom));
                manager.setVariable("bestRun", rift::Value::floating(fields->m_bestRun));
            }
        }

        void levelComplete() {
            PlayLayer::levelComplete();
            saveBestRun();
        }
        
        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            auto percentage = utils::getActualProgress(this);
            PlayLayer::destroyPlayer(player, object);
            if (object != m_anticheatSpike) {
                VariableManager::get().setVariable("lastDeath", rift::Value::from(percentage));
                saveBestRun();
            }
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            m_fields->m_runFrom = utils::getActualProgress(this);
        }
    };
}
