#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameManager.hpp>

namespace eclipse::hacks::Bypass {

    class UnlockShops : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.unlockshops")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Shops"; }
    };

    REGISTER_HACK(UnlockShops)

    class $modify(UnlockShopsGMHook, GameManager) {
        ADD_HOOKS_DELEGATE("bypass.unlockshops")

        bool getUGV(const char* key) {
            if (GameManager::getUGV(key))
                return true;

            // Shops:
            // 11 - Scratch Shop
            // 20 - Potbor Shop
            // 34 - Diamond Shopkeeper
            // 35 - Mechanic Shop
            return strcmp(key, "11") == 0 || strcmp(key, "20") == 0
                || strcmp(key, "34") == 0 || strcmp(key, "35") == 0;
        }
    };
}
