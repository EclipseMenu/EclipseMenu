#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameManager.hpp>

namespace eclipse::hacks::Bypass {

    class UnlockIcons : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Unlock Icons", "bypass.unlockicons");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Unlock Icons"; }
    };

    REGISTER_HACK(UnlockIcons)

    class $modify(GameManager) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("GameManager::isColorUnlocked");
            SAFE_PRIORITY("GameManager::isIconUnlocked");
        }

        bool isColorUnlocked(int key, UnlockType type) {
            if (config::get<bool>("bypass.unlockicons", false))
                return true;
            return GameManager::isColorUnlocked(key, type);
        }

        bool isIconUnlocked(int key, IconType type) {
            if (config::get<bool>("bypass.unlockicons", false))
                return true;
            return GameManager::isIconUnlocked(key, type);
        }
    };

}
