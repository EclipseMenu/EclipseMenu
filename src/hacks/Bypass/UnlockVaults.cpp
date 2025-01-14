#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/LevelPage.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/SecretLayer2.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(UnlockVaults) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.unlockvaults")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Vaults"; }
    };

    REGISTER_HACK(UnlockVaults)

    static bool s_bypassUGV = false;     // bypass once
    static bool s_bypassUGV2 = false;    // bypass twice in a row
    static bool s_bypassUGVSkip = false; // bypass after one time

    // i hate this (still better than patches/midhooks)
    class $modify(UnlockVaultsGMHook, GameManager) {
        ADD_HOOKS_DELEGATE("bypass.unlockvaults")

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
        ADD_HOOKS_DELEGATE("bypass.unlockvaults")

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
            return type == UnlockType::GJItem && key >= 1 && key <= 4;
        }
    };

    class $modify(UnlockVaultsCLHook, CreatorLayer) {
        ADD_HOOKS_DELEGATE("bypass.unlockvaults")

        void onSecretVault(cocos2d::CCObject* sender) {
            s_bypassGameStat = true;
            s_bypassGameStatValue = 51; // key 13 > 50
            CreatorLayer::onSecretVault(sender);
        }

        void onTreasureRoom(cocos2d::CCObject* sender) {
            s_bypassUGV = true;
            CreatorLayer::onTreasureRoom(sender);
        }

        bool init() override {
            s_bypassUGV = true;
            s_bypassGameStat = true;
            s_bypassGameStatValue = 51; // key 13 > 50
            return CreatorLayer::init();
        }
    };

    class $modify(UnlockVaultsOLHook, OptionsLayer) {
        ADD_HOOKS_DELEGATE("bypass.unlockvaults")

        void onSecretVault(cocos2d::CCObject* sender) {
            s_bypassGameStat = true;
            s_bypassGameStatValue = 11; // key 12 > 10
            OptionsLayer::onSecretVault(sender);
        }
    };

    class $modify(UnlockVaultsSL2Hook, SecretLayer2) {
        ADD_HOOKS_DELEGATE("bypass.unlockvaults")

        void onDoor(cocos2d::CCObject* sender) {
            s_bypassUGV = true;
            SecretLayer2::onDoor(sender);
        }
    };

    class $modify(UnlockVaultsLPHook, LevelPage) {
        void addSecretDoor() {
            s_bypassUGV = true;
            LevelPage::addSecretDoor();
        }
    };
}
