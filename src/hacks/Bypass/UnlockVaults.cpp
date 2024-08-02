#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/modify/LevelPage.hpp>

namespace eclipse::hacks::Bypass {

    class UnlockVaults : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Unlock Vaults", "bypass.unlockvaults")
                ->handleKeybinds()
                ->setDescription("Unlock all vaults and secrets in the game.");
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Vaults"; }
    };

    REGISTER_HACK(UnlockVaults)

    static bool s_bypassUGV = false; // bypass once
    static bool s_bypassUGV2 = false; // bypass twice in a row
    static bool s_bypassUGVSkip = false; // bypass after one time

    // i hate this (still better than patches/midhooks)
    class $modify(UnlockVaultsGMHook, GameManager) {
        bool getUGV(const char* key) {
            bool result = GameManager::getUGV(key);
            if (s_bypassUGVSkip) {
                s_bypassUGVSkip = false;
                s_bypassUGV = true;
                return result;
            } else if (s_bypassUGV2) {
                s_bypassUGV2 = false;
                s_bypassUGV = true;
                return true;
            } else if (s_bypassUGV) {
                s_bypassUGV = false;
                return true;
            }
            return result;
        }
    };

    static bool s_bypassGameStat = false; // bypass game stats
    static int s_bypassGameStatValue = 0; // return value

    class $modify(UnlockVaultsGSMHook, GameStatsManager) {
        int getStat(const char* key) {
            int value = GameStatsManager::getStat(key);
            if (!s_bypassGameStat) return value;
            s_bypassGameStat = false;
            return std::max(value, s_bypassGameStatValue);
        }

        bool isItemUnlocked(UnlockType type, int key) {
            if (GameStatsManager::isItemUnlocked(type, key))
                return true;

            // keys 1-3 + master emblem
            if (type == UnlockType::GJItem && key >= 1 && key <= 4)
                return config::get<bool>("bypass.unlockvaults", false);

            return false;
        }
    };

    class $modify(UnlockVaultsCLHook, CreatorLayer) {
        void onSecretVault(cocos2d::CCObject* sender) {
            if (config::get<bool>("bypass.unlockvaults", false)) {
                s_bypassGameStat = true;
                s_bypassGameStatValue = 51; // key 13 > 50
            }

            CreatorLayer::onSecretVault(sender);
        }

        void onTreasureRoom(cocos2d::CCObject* sender) {
            if (config::get<bool>("bypass.unlockvaults", false))
                s_bypassUGV = true;

            CreatorLayer::onTreasureRoom(sender);
        }

        bool init() override {
            if (config::get<bool>("bypass.unlockvaults", false)) {
                s_bypassUGV = true;
                s_bypassGameStat = true;
                s_bypassGameStatValue = 51; // key 13 > 50
            }

            return CreatorLayer::init();
        }
    };

    class $modify(UnlockVaultsOLHook, OptionsLayer) {
        void onSecretVault(cocos2d::CCObject* sender) {
            if (config::get<bool>("bypass.unlockvaults", false)) {
                s_bypassGameStat = true;
                s_bypassGameStatValue = 11; // key 12 > 10
            }

            OptionsLayer::onSecretVault(sender);
        }
    };

    class $modify(UnlockVaultsSL2Hook, SecretLayer2) {
        void onDoor(cocos2d::CCObject* sender) {
            if (config::get<bool>("bypass.unlockvaults", false))
                s_bypassUGV = true;

            SecretLayer2::onDoor(sender);
        }
    };

    class $modify(UnlockVaultsLPHook, LevelPage) {
        void addSecretDoor() {
            if (config::get<bool>("bypass.unlockvaults", false))
                s_bypassUGV = true;

            LevelPage::addSecretDoor();
        }
    };
}
