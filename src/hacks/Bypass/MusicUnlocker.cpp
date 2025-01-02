#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Cosmetic {
    class PracticeMusic : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.practicemusic")->handleKeybinds()->setDescription()
               ->callback([](bool v) {
                   utils::get<GameStatsManager>()->toggleEnableItem(UnlockType::GJItem, 17, v);
               });
        }

        [[nodiscard]] const char* getId() const override { return "Practice Music Sync"; }
    };

    class MusicCustomizer : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.musiccustomizer")
               ->handleKeybinds()
               ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Music Customizer"; }
    };

    REGISTER_HACK(PracticeMusic)
    REGISTER_HACK(MusicCustomizer)

    class $modify(MusicCustomizerGSMHook, GameStatsManager) {
        ENABLE_SAFE_HOOKS_ALL()

        bool isItemUnlocked(UnlockType type, int key) {
            if (GameStatsManager::isItemUnlocked(type, key)) return true;

            if (config::get<bool>("bypass.practicemusic", false))
                return type == UnlockType::GJItem && key == 17;

            if (config::get<bool>("bypass.musiccustomizer", false))
                return type == UnlockType::GJItem && key == 16;

            return false;
        }
    };
}
