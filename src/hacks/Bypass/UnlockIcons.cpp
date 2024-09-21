#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameManager.hpp>

namespace eclipse::hacks::Bypass {

    class UnlockIcons : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Unlock Icons", "bypass.unlockicons")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Icons"; }
    };

    REGISTER_HACK(UnlockIcons)

    class $modify(UnlockIconsGMHook, GameManager) {
        ENABLE_SAFE_HOOKS_ALL()

        bool isColorUnlocked(int key, UnlockType type) {
            if (GameManager::isColorUnlocked(key, type))
                return true;

            return config::get<bool>("bypass.unlockicons", false);
        }

        bool isIconUnlocked(int key, IconType type) {
            if (GameManager::isIconUnlocked(key, type))
                return true;

            return config::get<bool>("bypass.unlockicons", false);
        }
    };

}
