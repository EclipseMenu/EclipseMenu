//from prism its superseded anyways im lazy
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Cosmetic {

    class PracticeMusic : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Cosmetic");
            tab->addToggle("Practice Music", "player.practicemusic")->setDescription("Plays the level\'s normal music instead of the practice music in Practice Mode");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Practice Music"; }
    };

    REGISTER_HACK(PracticeMusic)

    class $modify(GameStatsManager) {
        bool isItemUnlocked(UnlockType p0, int p1) {
            // did i seriously not see this, all i changed was && to || hA!
            if (p0 != UnlockType::GJItem || p1 != 17) return GameStatsManager::isItemUnlocked(p0,p1);
            if (config::get<bool>("player.practicemusic", false) && p1 == 17) {
                return true;
            } else {
                return GameStatsManager::isItemUnlocked(p0,p1);
            }
        }
    };

}
