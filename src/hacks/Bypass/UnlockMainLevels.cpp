#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameLevelManager.hpp>

namespace eclipse::hacks::Bypass {

    class UnlockMainLevels : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.unlockmainlevels")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Unlock Main Levels"; }
    };

    REGISTER_HACK(UnlockMainLevels)

    class $modify(UnlockMainLevelsGJHook, GameLevelManager) {
        ENABLE_SAFE_HOOKS_ALL()

        GJGameLevel* getMainLevel(int levelID, bool dontGetLevelString) {
            auto level = GameLevelManager::getMainLevel(levelID, dontGetLevelString);
            if (level->m_requiredCoins > 0 && config::get<bool>("bypass.unlockmainlevels", false)) {
                level->m_requiredCoins = 0;
            }
            return level;
        }
    };

}
