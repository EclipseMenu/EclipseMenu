#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Cosmetic {

    class PracticeMusic : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");
            tab->addToggle("Practice Music Sync", "cosmetic.practicemusic");
        }

        [[nodiscard]] const char* getId() const override { return "Practice Music Sync"; }
    };

    REGISTER_HACK(PracticeMusic)

    class $modify(GameStatsManager) {
        bool isItemUnlocked(UnlockType p0, int p1) {
            if (p0 != UnlockType::GJItem || p1 != 17) return GameStatsManager::isItemUnlocked(p0, p1);

            if (!config::get<bool>("global.practicemusic", false)) {
                return GameStatsManager::isItemUnlocked(p0, p1);
            }

            GameStatsManager::isItemUnlocked(p0, p1);
            return true;
        }
    };
}
