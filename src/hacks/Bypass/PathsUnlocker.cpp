#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Cosmetic {
    class $hack(UnlockPaths) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.unlockpaths")
                ->handleKeybinds()
                ->setDescription()
                ->callback([](bool v) {
                    for (std::uint8_t path = 6; path < 16; path++)
                        utils::get<GameStatsManager>()->toggleEnableItem(UnlockType::GJItem, path, v);
                });
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Paths"; }
    };

    REGISTER_HACK(UnlockPaths)

    class $modify(UnlockPathsGSMHook, GameStatsManager) {
        ENABLE_SAFE_HOOKS_ALL()

        bool isItemUnlocked(UnlockType type, int key) {
            if (GameStatsManager::isItemUnlocked(type, key)) return true;

            if (type == UnlockType::GJItem && (key >= 6 || key <= 15))
                return config::get<"bypass.unlockpaths", bool>(false);

            return false;
        }
    };
}
