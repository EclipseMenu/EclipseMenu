#include "variables.hpp"
#include <modules/config/config.hpp>
#include <modules/utils/SingletonCache.hpp>
#include <utils.hpp>

#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/loader/Mod.hpp>
#include <Geode/Loader.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

#include <rift/config.hpp>

namespace eclipse::labels {

    static std::vector<EffectGameObject*> s_coins;

    rift::Value getConfigValue(std::string key) {
        if (!config::has(key)) {
            return {};
        }
        switch (config::getType(key)) {
            case nlohmann::detail::value_t::string:
                return config::get<std::string>(key).unwrap();
            case nlohmann::detail::value_t::boolean:
                return config::get<bool>(key).unwrap();
            case nlohmann::detail::value_t::number_integer:
                return config::get<int>(key).unwrap();
            case nlohmann::detail::value_t::number_float:
                return config::get<float>(key).unwrap();
            default:
                return {};
        }
    }

    $on_mod(Loaded) {
        rift::Config::get().makeFunction("cfg", getConfigValue);
    }

    VariableManager& VariableManager::get() {
        static VariableManager instance = []{
            VariableManager manager;
            manager.init();
            return manager;
        }();
        return instance;
    }

    void VariableManager::init() {
        auto* mod = geode::Mod::get();
        auto* loader = geode::Loader::get();

        // Mod variables
        m_variables["modVersion"] = mod->getVersion().toNonVString();
        m_variables["geodeVersion"] = geode::Loader::get()->getVersion().toNonVString();
        m_variables["platform"] = GEODE_WINDOWS("Windows")
                                        GEODE_ANDROID("Android")
                                        GEODE_MACOS("macOS")
                                        GEODE_IOS("iOS");
        m_variables["gameVersion"] = loader->getGameVersion();
        auto allMods = loader->getAllMods();
        m_variables["totalMods"] = static_cast<int64_t>(allMods.size());
        m_variables["enabledMods"] = std::ranges::count_if(allMods, [](auto* mod) { return mod->shouldLoad(); });

        // Emojis :D
        m_variables["starEmoji"] = "⭐";
        m_variables["moonEmoji"] = "🌙";
        m_variables["heartEmoji"] = "❤️";
        m_variables["brokenHeartEmoji"] = "💔";
        m_variables["checkEmoji"] = "✅";
        m_variables["crossEmoji"] = "❌";
        m_variables["exclamationEmoji"] = "❗";
        m_variables["questionEmoji"] = "❓";
        m_variables["fireEmoji"] = "🔥";
        m_variables["snowflakeEmoji"] = "❄️";
        m_variables["catEmoji"] = "🐱";
        m_variables["dogEmoji"] = "🐶";
        m_variables["speakingHeadEmoji"] = "🗣️";
        m_variables["robotEmoji"] = "🤖";
        m_variables["alienEmoji"] = "👽";
        m_variables["ghostEmoji"] = "👻";
        m_variables["skullEmoji"] = "💀";
        m_variables["babyEmoji"] = "👶";
        m_variables["likeEmoji"] = "👍";
        m_variables["dislikeEmoji"] = "👎";
        m_variables["personInSteamyRoomEmoji"] = "🧖";
        m_variables["eclipseEmoji"] = "🌗";
        m_variables["nerdEmoji"] = "🤓";
        m_variables["sobEmoji"] = "😭";
        m_variables["explodingHeadEmoji"] = "🤯";

        // special emojis
        m_variables["emojis"] = rift::Object {
            { "userCoin", rift::Array { "🛞", "🔵" } },
            { "secretCoin", rift::Array { "⭕", "🟡" } },
            { "startPos", "🧿" },
            { "practice", "♦️" },
        };

        // Fetch everything else
        m_variables["fps"] = 0.f;
        m_variables["realFps"] = 0.f;
        m_variables["tps"] = 0.f;
        refetch();
    }

    void VariableManager::setVariable(const std::string& name, const rift::Value& value) {
        m_variables[name] = value;
    }

    rift::Value VariableManager::getVariable(const std::string& name) const {
        auto it = m_variables.find(name);
        if (it == m_variables.end()) return {};
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
        m_variables["realFps"] = fps;
        m_variables["fps"] = accumulateFPS(fps);
    }

    void VariableManager::fetchGeneralData() {
        auto* gameManager = utils::get<GameManager>();
        m_variables["username"] = rift::Value::string(gameManager->m_playerName);
        m_variables["cubeIcon"] = utils::getPlayerIcon(PlayerMode::Cube);
        m_variables["shipIcon"] = utils::getPlayerIcon(PlayerMode::Ship);
        m_variables["ballIcon"] = utils::getPlayerIcon(PlayerMode::Ball);
        m_variables["ufoIcon"] = utils::getPlayerIcon(PlayerMode::UFO);
        m_variables["waveIcon"] = utils::getPlayerIcon(PlayerMode::Wave);
        m_variables["robotIcon"] = utils::getPlayerIcon(PlayerMode::Robot);
        m_variables["spiderIcon"] = utils::getPlayerIcon(PlayerMode::Spider);
        m_variables["swingIcon"] = utils::getPlayerIcon(PlayerMode::Swing);
    }

    void VariableManager::fetchTimeData() {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto localTime = std::localtime(&time);
        m_variables["hour"] = localTime->tm_hour;
        m_variables["minute"] = localTime->tm_min;
        m_variables["second"] = localTime->tm_sec;
        m_variables["day"] = localTime->tm_mday;
        m_variables["month"] = localTime->tm_mon + 1;
        m_variables["monthName"] = utils::getMonthName(localTime->tm_mon);
        m_variables["year"] = localTime->tm_year + 1900;
        m_variables["clock"] = utils::getClock();
        m_variables["clock12"] = utils::getClock(true);
    }

    void VariableManager::fetchHacksData() {
        m_variables["isCheating"] = config::getTemp("hasCheats", false);
        m_variables["noclip"] = config::get("player.noclip", false);
        m_variables["speedhack"] = config::get("global.speedhack.toggle", false);
        m_variables["speedhackSpeed"] = config::get("global.speedhack", 1.f);
        m_variables["framestepper"] = config::get("player.framestepper", false);
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

        auto levelID = level->m_levelID.value();
        bool isRobtopLevel = (levelID > 0 && levelID < 100) || (levelID >= 3001 && levelID <= 6000);
        auto levelDifficulty = getLevelDifficulty(level);
        m_variables["levelID"] = levelID;
        m_variables["levelName"] = rift::Value::string(level->m_levelName);
        m_variables["levelDescription"] = cachedBase64Decode(level->m_levelDesc);
        m_variables["author"] = rift::Value::string(isRobtopLevel ? "RobTop" : level->m_creatorName);
        m_variables["isRobtopLevel"] = isRobtopLevel;
        m_variables["levelAttempts"] = level->m_attempts.value();
        m_variables["levelStars"] = level->m_stars.value();
        m_variables["difficulty"] = getLevelDifficultyString(levelDifficulty);
        m_variables["difficultyKey"] = getLevelDifficultyKey(levelDifficulty);
        m_variables["practicePercent"] = level->m_practicePercent;
        m_variables["bestPercent"] = level->m_normalPercent.value();
        m_variables["bestTime"] = level->m_bestTime;
        if (utils::get<GJBaseGameLayer>()->m_isPlatformer)
            m_variables["best"] =formatTime(level->m_bestTime);
        else
            m_variables["best"] = level->m_normalPercent.value();
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
                m_variables["gamemode"] = utils::gameModeName(gamemode);
                m_variables["playerIcon"] = utils::getPlayerIcon(gamemode);
            }
            return;
        }

        m_variables[isPlayer2 ? "player2X" : "playerX"] = player->m_position.x;
        m_variables[isPlayer2 ? "player2Y" : "playerY"] = player->m_position.y;

        if (!isPlayer2) {
            auto gamemode = utils::getGameMode(player);
            m_variables["gamemode"] = utils::gameModeName(gamemode);
            m_variables["playerIcon"] = utils::getPlayerIcon(gamemode);
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

        m_variables["attempt"] = gameLayer->m_attempts;
        m_variables["isTestMode"] = gameLayer->m_isTestMode;
        m_variables["isPracticeMode"] = gameLayer->m_isPracticeMode;
        m_variables["isPlatformer"] = gameLayer->m_isPlatformer;
        m_variables["levelTime"] = gameLayer->m_gameState.m_levelTime;
        m_variables["levelLength"] = gameLayer->m_levelLength;
        m_variables["levelDuration"] = gameLayer->m_level->m_timestamp / 240.f;
        m_variables["time"] = utils::formatTime(gameLayer->m_gameState.m_levelTime);
        m_variables["frame"] = gameLayer->m_gameState.m_currentProgress;
        m_variables["frameReal"] = gameLayer->m_gameState.m_levelTime * utils::getTPS();
        m_variables["isDead"] = gameLayer->m_player1->m_isDead;
        m_variables["isDualMode"] = gameLayer->m_player2 != nullptr && gameLayer->m_player2->isRunning(); // can m_isDualMode be added already
        m_variables["noclipDeaths"] = config::getTemp("noclipDeaths", 0);
        m_variables["noclipAccuracy"] = config::getTemp("noclipAccuracy", 100.f);
        m_variables["progress"] = utils::getActualProgress(gameLayer);
        m_variables["timeWarp"] = gameLayer->m_gameState.m_timeWarp;
        m_variables["gravity"] = gameLayer->m_player1->m_gravityMod;
        m_variables["activeObjects"] = gameLayer->m_activeObjects;
        m_variables["gradients"] = gameLayer->m_activeGradients;
        m_variables["particleCount"] = gameLayer->m_particleCount;

        auto fmod = utils::get<FMODAudioEngine>();
        m_variables["songsCount"] = fmod->countActiveMusic();
        m_variables["sfxCount"] = fmod->countActiveEffects();

        m_variables["moveTriggerCount"] = gameLayer->m_movedCountDisplay;
        m_variables["rotateTriggerCount"] = gameLayer->m_rotatedCountDisplay;
        m_variables["scaleTriggerCount"] = gameLayer->m_scaledCountDisplay;
        m_variables["followTriggerCount"] = gameLayer->m_followedCountDisplay;

        m_variables["areaMoveTrigger"] = gameLayer->m_areaMovedCountTotalDisplay;
        m_variables["areaMoveTriggerTotal"] = gameLayer->m_areaMovedCountDisplay;
        m_variables["areaRotateTrigger"] = gameLayer->m_areaRotatedCountTotalDisplay;
        m_variables["areaRotateTriggerTotal"] = gameLayer->m_areaRotatedCountDisplay;
        m_variables["areaScaleTrigger"] = gameLayer->m_areaScaledCountTotalDisplay;
        m_variables["areaScaleTriggerTotal"] = gameLayer->m_areaScaledCountDisplay;
        m_variables["areaColOpTrigger"] = gameLayer->m_areaColorCountTotalDisplay;
        m_variables["areaColOpTriggerTotal"] = gameLayer->m_areaColorCountDisplay;

        auto coinsArr = rift::Array();
        for (int i = 0; i < s_coins.size(); ++i) {
            auto coinKey = gameLayer->m_level->getCoinKey(i + 1);
            bool saved = false;

            if (gameLayer->m_level->m_levelType == GJLevelType::Local) {
                saved = utils::get<GameStatsManager>()->hasSecretCoin(coinKey);
            } else {
                saved = utils::get<GameStatsManager>()->hasUserCoin(coinKey);
            }

            if (saved) {
                coinsArr.push_back(2);
                continue;
            }

            auto pickedUp = gameLayer->m_collectedItems->objectForKey(std::to_string(i + 1)) != nullptr;
            coinsArr.push_back(pickedUp ? 1 : 0);
        }
        m_variables["coins"] = coinsArr;

        if (auto* pl = utils::get<PlayLayer>()) {
            m_variables["editorMode"] = false;
            m_variables["realProgress"] = pl->getCurrentPercent();
            m_variables["objects"] = gameLayer->m_level->m_objectCount.value();
        } else if (auto* ed = utils::get<LevelEditorLayer>()) {
            m_variables["editorMode"] = true;
            m_variables["realProgress"] = 0.f;
            m_variables["objects"] = static_cast<int64_t>(ed->m_objects->count());
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
        fetchGameplayData(utils::get<GJBaseGameLayer>());
    }

    class $modify(LabelsGJBGLHook, GJBaseGameLayer) {
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            static time_t s_lastUpdate = utils::getTimestamp();
            static size_t s_frames = 0;
            s_frames++;
            auto now = utils::getTimestamp();
            auto diff = now - s_lastUpdate;
            constexpr time_t interval = 250;
            if (diff >= interval) {
                auto tps = s_frames / (diff / 1000.0);
                VariableManager::get().setVariable("tps", tps);
                s_lastUpdate = now;
                s_frames = 0;
            }
        }

        void pickupItem(EffectGameObject* obj) {
            GJBaseGameLayer::pickupItem(obj);
            geode::log::debug("pickupItem: {}", obj->m_secretCoinID);
        }
    };

    class $modify(VariablesPLHook, PlayLayer) {
        struct Fields {
            float m_runFrom = 0.f;
            float m_lastRunFrom = 0.f;
            float m_bestRun = 0.f;
            float m_lastBestRun = 0.f;
        };

        void addObject(GameObject* obj) {
            PlayLayer::addObject(obj);
            auto id = obj->m_objectID;

            if (id == 142 || id == 1329)
                s_coins.push_back(static_cast<EffectGameObject*>(obj));
        }

        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            s_coins.clear();
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

            auto& manager = VariableManager::get();
            manager.setVariable("runFrom", 0.f);
            manager.setVariable("bestRun", 0.f);

            return true;
        }

        void saveBestRun() {
            auto fields = m_fields.self();
            fields->m_bestRun = utils::getActualProgress(this);
            if ((fields->m_bestRun - fields->m_runFrom) >= (fields->m_lastBestRun - fields->m_lastRunFrom)) {
                fields->m_lastBestRun = fields->m_bestRun;
                fields->m_lastRunFrom = fields->m_runFrom;
                auto& manager = VariableManager::get();
                manager.setVariable("runFrom", fields->m_runFrom);
                manager.setVariable("bestRun", fields->m_bestRun);
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
                VariableManager::get().setVariable("lastDeath", percentage);
                saveBestRun();
            }
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            auto from = utils::getActualProgress(this);
            m_fields->m_runFrom = from;
            VariableManager::get().setVariable("runStart", from);
        }
    };
}
