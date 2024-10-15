#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Cosmetic {

    class PracticeMusic : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Practice Music Sync", "bypass.practicemusic")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Practice Music Sync"; }
    };

    class MusicCustomizer : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Music Customizer", "bypass.musiccustomizer")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Music Customizer"; }
    };

    REGISTER_HACK(PracticeMusic)
    REGISTER_HACK(MusicCustomizer)

    class $modify(MusicCustomizerGSMHook, GameStatsManager) {
        ENABLE_SAFE_HOOKS_ALL()

        bool isItemUnlocked(UnlockType type, int key) {
            if (GameStatsManager::isItemUnlocked(type, key))
                return true;

            if (config::get<bool>("bypass.practicemusic", false))
                return type == UnlockType::GJItem && key == 17;

            if (config::get<bool>("bypass.musiccustomizer", false))
                return type == UnlockType::GJItem && key == 16;
            
            return false;
        }
    };
}
