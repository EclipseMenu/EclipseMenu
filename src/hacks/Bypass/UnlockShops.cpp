#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameManager.hpp>

namespace eclipse::hacks::Bypass {

    class UnlockShops : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Unlock Shops", "bypass.unlockshops")
                ->handleKeybinds()
                ->setDescription("Unlock all shops in the game");
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Shops"; }
    };

    REGISTER_HACK(UnlockShops)

    class $modify(UnlockShopsGMHook, GameManager) {
        bool getUGV(const char* key) {
            if (GameManager::getUGV(key))
                return true;

            if (config::get<bool>("bypass.unlockshops", false)) {
                // Shops:
                // 11 - Scratch Shop
                // 20 - Potbor Shop
                // 34 - Diamond Shopkeeper
                // 35 - Mechanic Shop
                if (strcmp(key, "11") == 0 || strcmp(key, "20") == 0 || strcmp(key, "34") == 0 || strcmp(key, "35") == 0)
                    return true;
            }

            return false;
        }
    };
}
