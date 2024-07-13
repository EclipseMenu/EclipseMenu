#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Cosmetic {

    class PracticeMusic : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Cosmetic");
            tab->addToggle("Practice Music Sync", "cosmetic.practicemusic")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Practice Music Sync"; }
    };

    REGISTER_HACK(PracticeMusic)

    class $modify(GameStatsManager) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("GameStatsManager::isItemUnlocked");
        }

        bool isItemUnlocked(UnlockType type, int key) {
            if (GameStatsManager::isItemUnlocked(type, key))
                return true;

            if (config::get<bool>("cosmetic.practicemusic", false))
                return type == UnlockType::GJItem && key == 17;
            
            return false;
        }
    };
}
