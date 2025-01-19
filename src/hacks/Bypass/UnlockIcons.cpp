#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(UnlockIcons) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.unlockicons")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Icons"; }
    };

    REGISTER_HACK(UnlockIcons)

    class $modify(UnlockIconsGMHook, GameManager) {
        ENABLE_SAFE_HOOKS_ALL()

        bool isColorUnlocked(int key, UnlockType type) {
            if (GameManager::isColorUnlocked(key, type)) return true;

            return config::get<bool>("bypass.unlockicons", false);
        }

        bool isIconUnlocked(int key, IconType type) {
            if (GameManager::isIconUnlocked(key, type)) return true;

            return config::get<bool>("bypass.unlockicons", false);
        }
    };

    class $modify(UnlockIconsGSMHook, GameStatsManager) {
        ENABLE_SAFE_HOOKS_ALL()

        bool isItemUnlocked(UnlockType type, int key) {
            if (GameStatsManager::isItemUnlocked(type, key)) return true;

            return config::get<bool>("bypass.unlockicons", false);
        }
    };
}
