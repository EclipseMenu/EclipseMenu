#include "variables.hpp"
#include <utils.hpp>
#include <modules/config/config.hpp>
#include <modules/utils/SingletonCache.hpp>

#include <Geode/Loader.hpp>
#include <Geode/binding/GameManager.hpp>
#include <Geode/binding/GJGameLevel.hpp>
#include <Geode/binding/LevelEditorLayer.hpp>
#include <Geode/binding/PlayerObject.hpp>
#include <Geode/binding/PlayLayer.hpp>
#include <Geode/loader/Mod.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <rift/config.hpp>

#include <dankmeme.globed2/include/globed.hpp>

namespace eclipse::labels {
    static std::vector<EffectGameObject*> s_coins;

    rift::Value getConfigValue(std::string const& key) {
        if (!config::has(key)) {
            return {};
        }
        switch (config::getType(key)) {
            case nlohmann::detail::value_t::string: return config::get<std::string>(key).unwrap();
            case nlohmann::detail::value_t::boolean: return config::get<bool>(key).unwrap();
            case nlohmann::detail::value_t::number_integer: return config::get<int>(key).unwrap();
            case nlohmann::detail::value_t::number_float: return config::get<float>(key).unwrap();
            default: return {};
        }
    }

    $on_mod(Loaded) {
        rift::Config::get().makeFunction("cfg", getConfigValue);
    }

    VariableManager& VariableManager::get() {
        static VariableManager instance = [] {
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
        m_variables["enabledMods"] = rift::Value::integer(
            std::ranges::count_if(allMods, [](auto* mod) { return mod->shouldLoad(); })
        );

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
        m_variables["sleepingAccommodationEmoji"] = "🛌";
        m_variables["eclipseEmoji"] = "🌗";
        m_variables["nerdEmoji"] = "🤓";
        m_variables["sobEmoji"] = "😭";
        m_variables["explodingHeadEmoji"] = "🤯";
        m_variables["diamondEmoji"] = "💎";
        m_variables["diamondShardsEmoji"] = "💠";
        m_variables["geodeEmoji"] = "🪩";
        m_variables["orbsEmoji"] = "🔹";
        m_variables["emoji100"] = "💯";
        m_variables["shushingFaceEmoji"] = "🤫";
        m_variables["bearEmoji"] = "🐻";
        m_variables["foxEmoji"] = "🦊";
        m_variables["gunEmoji"] = "🔫";
        m_variables["faceWithRaisedEyebrowEmoji"] = "🤨";
        m_variables["moyaiEmoji"] = "🗿";
        m_variables["screamEmoji"] = "😱";
        m_variables["sunglassesEmoji"] = "😎";
        m_variables["heartEyesEmoji"] = "😍";
        m_variables["smilingImpEmoji"] = "😈";
        m_variables["clownEmoji"] = "🤡";
        m_variables["fishEmoji"] = "🐟";
        m_variables["sparklesEmoji"] = "✨";
        m_variables["partyPopperEmoji"] = "🎉";
        m_variables["pigEmoji"] = "🐷";
        m_variables["moonWithFaceEmoji"] = "🌚";
        m_variables["fullMoonFaceEmoji"] = "🌝";
        m_variables["sunWithFaceEmoji"] = "🌞";
        m_variables["monkeyEmoji"] = "🐵";
        m_variables["seeNoEvilMonkey"] = "🙈";
        m_variables["hearNoEvilMonkey"] = "🙉";
        m_variables["speakNoEvilMonkey"] = "🙊";
        m_variables["crownEmoji"] = "👑";
        m_variables["prayEmoji"] = "🙏";

        // special emojis
        m_variables["emojis"] = rift::Object{
            {"userCoin", rift::Array{"🛞", "🔵"}},
            {"secretCoin", rift::Array{"⭕", "🟡"}},
            {"startPos", "🧿"},
            {"practice", "♦️"},
        };

        // regional indicators

        m_variables["ri"] = rift::Object{
            {"a", "🇦"}, {"b", "🇧"}, {"c", "🇨"}, {"d", "🇩"},
            {"e", "🇪"}, {"f", "🇫"}, {"g", "🇬"}, {"h", "🇭"},
            {"i", "🇮"}, {"j", "🇯"}, {"k", "🇰"}, {"l", "🇱"},
            {"m", "🇲"}, {"n", "🇳"}, {"o", "🇴"}, {"p", "🇵"},
            {"q", "🇶"}, {"r", "🇷"}, {"s", "🇸"}, {"t", "🇹"},
            {"u", "🇺"}, {"v", "🇻"}, {"w", "🇼"}, {"x", "🇽"},
            {"y", "🇾"}, {"z", "🇿"},
            {"num", rift::Array{
                "0️⃣", "1️⃣", "2️⃣", "3️⃣", "4️⃣",
                "5️⃣", "6️⃣", "7️⃣", "8️⃣", "9️⃣"
            }},
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

    const char* getLevelRatingString(GJGameLevel* level) {
        if (!level) return "Unknown";
        int featured = level->m_featured;
        int epic = level->m_isEpic;
        switch (epic) {
            default: // Not epic (featured, rated, unrated)
                if (level->m_stars.value() == 0 && featured <= 0) {
                    return "Unrated";
                } else {
                    return (featured <= 0) ? "Rated" : "Featured";
                }
            case 1: // Epic
                return "Epic";
            case 2: // Legendary
                return "Legendary";
            case 3: // Mythic
                return "Mythic";
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

    constexpr int FIRST_PATH = 30;
    constexpr int LAST_PATH = 39;
    static const std::array<std::string, 10> PATH_NAMES = {
        "fire", "ice", "poison", "shadow", "lava",
        "earth", "blood", "metal", "light", "soul"
    };
    static const std::array<std::string, 15> STAT_NAMES_1 = {
        "totalJumps", "totalAttempts", "completedLevels", "completedOnlineLevels",
        "demons", "stars", "completedMapPacks", "goldCoins", "playersDestroyed",
        "likedLevels", "ratedLevels", "userCoins", "diamonds", "orbs", "dailies",
    };

    inline int getStatFromDictionary(cocos2d::CCDictionary* dict, gd::string const& key) {
        auto& str = dict->valueForKey(key)->m_sString;
        if (str.empty()) return 0;
        if (auto res = geode::utils::numFromString<int>(str)) {
            return res.unwrap();
        }
        return 0;
    }

    // when m_usePlayerStatsCCDictionary is true
    void playerStatsWithCCDict(rift::Object& variables) {
        auto gsm = utils::get<GameStatsManager>();
        auto playerStats = gsm->m_playerStats;

        auto& stats = variables["stats"];
        for (int i = 0; i < STAT_NAMES_1.size(); i++) {
            stats[STAT_NAMES_1[i]] = getStatFromDictionary(playerStats, fmt::to_string(i + 1));
        }

        auto& stats2 = const_cast<rift::Object&>(stats.getObject()); // FIXME: add this api to rift
        stats2["keys"] = getStatFromDictionary(playerStats, "21");
        stats2["totalOrbs"] = getStatFromDictionary(playerStats, "22");
        stats2["moons"] = getStatFromDictionary(playerStats, "28");
        stats2["diamondShards"] = getStatFromDictionary(playerStats, "29");
        stats2["gauntlets"] = getStatFromDictionary(playerStats, "40");
        stats2["lists"] = getStatFromDictionary(playerStats, "41");

        auto& shards = stats2["shards"];
        for (int i = 16; i <= 27; i++) {
            if (i == 21 || i == 22) continue; // skip demon keys and total orbs

            int index = i - 16;
            if (i > 22) index -= 2;

            shards[PATH_NAMES[index]] = getStatFromDictionary(playerStats, fmt::to_string(i));
        }

        auto& paths = variables["paths"];
        for (int i = 0; i <= PATH_NAMES.size(); i++) {
            paths[PATH_NAMES[i]] = getStatFromDictionary(playerStats, fmt::to_string(i + FIRST_PATH));
        }
    }

    inline int getStatIndex(int i, gd::unordered_map<int, int> const& seeds, gd::unordered_map<int, int> const& rands) {
        auto rand = rands.find(i);
        auto seed = seeds.find(i);
        if (rand != rands.end() && seed != seeds.end()) {
            return rand->second - seed->second;
        }
        return 0;
    }

    // when m_usePlayerStatsCCDictionary is false
    void playerStatsWithMaps(rift::Object& variables) {
        auto gsm = utils::get<GameStatsManager>();
        auto const& seeds = gsm->m_playerStatsSeedMap;
        auto const& rands = gsm->m_playerStatsRandMap;

        auto& stats = variables["stats"];
        for (int i = 1; i <= STAT_NAMES_1.size(); i++) {
            stats[STAT_NAMES_1[i - 1]] = getStatIndex(i, seeds, rands);
        }

        auto& stats2 = const_cast<rift::Object&>(stats.getObject()); // FIXME: add this api to rift
        stats2["keys"] = getStatIndex(21, seeds, rands);
        stats2["totalOrbs"] = getStatIndex(22, seeds, rands);
        stats2["moons"] = getStatIndex(28, seeds, rands);
        stats2["diamondShards"] = getStatIndex(29, seeds, rands);
        stats2["gauntlets"] = getStatIndex(40, seeds, rands);
        stats2["lists"] = getStatIndex(41, seeds, rands);

        auto& shards = stats2["shards"];
        for (int i = 16; i <= 27; i++) {
            if (i == 21 || i == 22) continue; // skip demon keys and total orbs

            int index = i - 16;
            if (i > 22) index -= 2;

            shards[PATH_NAMES[index]] = getStatIndex(i, seeds, rands);
        }

        auto& paths = variables["paths"];
        for (int i = FIRST_PATH; i <= LAST_PATH; i++) {
            paths[PATH_NAMES[i - FIRST_PATH]] = getStatIndex(i, seeds, rands);
        }
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

        auto gsm = utils::get<GameStatsManager>();
        if (gsm->m_usePlayerStatsCCDictionary) {
            playerStatsWithCCDict(m_variables);
        } else {
            playerStatsWithMaps(m_variables);
        }

        if (gsm->m_activePath >= FIRST_PATH && gsm->m_activePath <= LAST_PATH) {
            m_variables["currentPath"] = PATH_NAMES[gsm->m_activePath - FIRST_PATH];
        } else {
            m_variables["currentPath"] = {};
        }

    }

    void VariableManager::fetchTimeData() {
        auto time = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        auto localTime = fmt::localtime(time);
        m_variables["hour"] = localTime.tm_hour;
        m_variables["minute"] = localTime.tm_min;
        m_variables["second"] = localTime.tm_sec;
        m_variables["day"] = localTime.tm_mday;
        m_variables["month"] = localTime.tm_mon + 1;
        m_variables["monthName"] = utils::getMonthName(localTime.tm_mon);
        m_variables["year"] = localTime.tm_year + 1900;
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

    constexpr bool isRobTopLevelID(int levelID) {
        return (levelID >= 1 && levelID <= 22) // Official levels
               || (levelID >= 5001 && levelID <= 5004) // Platformer levels
               || (levelID >= 1001 && levelID <= 1003) // GD Meltdown
               || (levelID >= 2001 && levelID <= 2010) // GD World
               || (levelID >= 4001 && levelID <= 4003) // GD SubZero
               || levelID == 3001; // "The Challenge"
    }

    int getTotalOrbsForLevel(GJGameLevel* level, int levelID) {
        static int s_lastLevelID = -1;
        static int s_lastTotalOrbs = 0;
        if (levelID == s_lastLevelID) return s_lastTotalOrbs;
        s_lastLevelID = levelID;
        s_lastTotalOrbs = utils::get<GameStatsManager>()->getBaseCurrencyForLevel(level);
        return s_lastTotalOrbs;
    }

    int getCurrentOrbsForLevel(GJGameLevel* level, int levelID) {
        auto gsm = utils::get<GameStatsManager>();
        auto totalOrbs = getTotalOrbsForLevel(level, levelID);
        auto dailyId = level->m_dailyID.value();

        cocos2d::CCDictionary* currencyScores;
        if (dailyId <= 0) {
            if (level->m_levelType == GJLevelType::Main) {
                currencyScores = gsm->m_mainCurrencyScores;
            } else if (level->m_gauntletLevel) {
                currencyScores = gsm->m_gauntletCurrencyScores;
            } else {
                currencyScores = gsm->m_onlineCurrencyScores;
            }
            dailyId = levelID;
        } else {
            currencyScores = gsm->m_timelyCurrencyScores;
        }

        auto& str = currencyScores->valueForKey(fmt::to_string(dailyId))->m_sString;
        if (str.empty()) return 0;
        if (auto res = geode::utils::numFromString<int>(str)) {
            auto resValue = res.unwrap();
            if (resValue >= 100) return totalOrbs * 1.25;
            return std::floor(totalOrbs * (resValue / 100.f));
        }
        return 0;
    }

    void VariableManager::fetchLevelData(GJGameLevel* level) {
        if (!level) {
            // Reset all level variables
            constexpr std::array keys = {
                "levelID", "levelName", "levelDescription", "author",
                "isRobtopLevel", "levelAttempts", "levelStars", "levelRating",
                "difficulty", "difficultyKey", "practicePercent",
                "bestPercent", "bestTime", "best"
            };
            for (const auto& key : keys) {
                removeVariable(key);
            }
            return;
        }

        auto levelID = level->m_levelID.value();
        bool isRobtopLevel = isRobTopLevelID(levelID);
        auto levelDifficulty = getLevelDifficulty(level);
        m_variables["levelID"] = levelID;
        m_variables["levelName"] = rift::Value::string(level->m_levelName);
        m_variables["levelDescription"] = cachedBase64Decode(level->m_levelDesc);
        m_variables["author"] = rift::Value::string(isRobtopLevel ? "RobTop" : level->m_creatorName);
        m_variables["isRobtopLevel"] = isRobtopLevel;
        m_variables["levelAttempts"] = level->m_attempts.value();
        m_variables["levelStars"] = level->m_stars.value();
        m_variables["levelRating"] = getLevelRatingString(level);
        m_variables["difficulty"] = getLevelDifficultyString(levelDifficulty);
        m_variables["difficultyKey"] = getLevelDifficultyKey(levelDifficulty);
        m_variables["practicePercent"] = level->m_practicePercent;
        m_variables["bestPercent"] = level->m_normalPercent.value();
        m_variables["bestTime"] = level->m_bestTime;
        if (utils::get<GJBaseGameLayer>()->m_isPlatformer)
            m_variables["best"] =formatTime(level->m_bestTime);
        else
            m_variables["best"] = level->m_normalPercent.value();

        m_variables["totalLevelOrbs"] = static_cast<int>(getTotalOrbsForLevel(level, levelID) * 1.25);
        m_variables["levelOrbs"] = getCurrentOrbsForLevel(level, levelID);
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
        m_variables["isDualMode"] = gameLayer->m_player2 != nullptr && gameLayer->m_player2->isRunning();
        m_variables["noclipDeaths"] = config::getTemp("noclipDeaths", 0);
        m_variables["noclipAccuracy"] = config::getTemp("noclipAccuracy", 100.f);
        m_variables["progress"] = utils::getActualProgress(gameLayer);
        m_variables["timeWarp"] = gameLayer->m_gameState.m_timeWarp;
        m_variables["gravity"] = gameLayer->m_player1->m_gravityMod;
        m_variables["activeObjects"] = gameLayer->m_activeObjectsCount;
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

            if (gameLayer->m_level->m_levelType == GJLevelType::Main) {
                saved = utils::get<GameStatsManager>()->hasSecretCoin(coinKey);
            } else {
                saved = utils::get<GameStatsManager>()->hasUserCoin(coinKey);
            }

            if (saved) {
                coinsArr.push_back(2);
                continue;
            }

            auto pickedUp = gameLayer->m_collectedItems->objectForKey(fmt::to_string(i + 1)) != nullptr;
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

    void fetchGlobedData(rift::Object& variables) {
        auto& globed = variables["globed"];
        globed[std::string("enabled")] = geode::Loader::get()->isModLoaded("dankmeme.globed2");
        if (globed[std::string("enabled")]) {
            // net
            globed[std::string("isConnected")] = globed::net::isConnected().unwrapOrDefault();
            globed[std::string("ping")] = (int)globed::net::getPing().unwrapOrDefault();
            globed[std::string("tps")] = (int)globed::net::getServerTps().unwrapOrDefault();

            // suggestion for daniel meme to add:
            // serverName
            // roomName
            // roomId
            // other room things
            globed[std::string("playersOnline")] = (int)globed::player::playersOnline().unwrapOrDefault();
            globed[std::string("playersOnLevel")] = (int)globed::player::playersOnLevel().unwrapOrDefault();
        }
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

        // Globed
        fetchGlobedData(m_variables);
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

        bool shouldSave() {
            bool safeMode = config::get<"global.safemode", bool>(false);
            bool autoSafeMode = config::get<"global.autosafemode", bool>(false);
            bool freezeBestRun = config::get<"global.safemode.freeze_best_run", bool>(false);
            auto trippedLastAttempt = config::getTemp<bool>("trippedSafeMode", false);
            auto hasCheats = config::getTemp<bool>("hasCheats", false);

            if((autoSafeMode && freezeBestRun && (hasCheats || trippedLastAttempt))
                || (safeMode && freezeBestRun)) return false;

            if(config::get<"player.noclip", bool>(false) && !m_levelEndAnimationStarted && !m_hasCompletedLevel) {
                if (config::get<"player.noclip.acclimit.toggle", bool>(false)) {
                    float acc = config::getTemp<float>("noclipAccuracy", 100.f);
                    float limit = config::get<"player.noclip.acclimit", float>(95.f);
                    if (acc >= limit)
                        return false;
                }
                if (config::get<"player.noclip.deathlimit.toggle", bool>(false)) {
                    int deaths = config::getTemp<int>("noclipDeaths", 0);
                    int limit = config::get<int>("player.noclip.deathlimit", 2);
                    if (deaths < limit)
                        return false;
                }
            }

            return true;
        }

        void saveBestRun() {
            auto fields = m_fields.self();
            fields->m_bestRun = utils::getActualProgress(this);
            if (shouldSave() && (fields->m_bestRun - fields->m_runFrom) >= (fields->m_lastBestRun - fields->m_lastRunFrom)) {
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
