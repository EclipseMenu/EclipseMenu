#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoSongDownload : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Auto Song Download", "level.autosongdownload")
                ->handleKeybinds()
                ->setDescription("Auto-download song and SFX files when viewing an online level. (Created by Uproxide)");
        }

        [[nodiscard]] const char* getId() const override { return "Auto Song Download"; }
    };

    REGISTER_HACK(AutoSongDownload)

    class $modify(AutoSongDownloadLILHook, LevelInfoLayer) {
        ADD_HOOKS_DELEGATE("level.autosongdownload")

        void levelDownloadFinished(GJGameLevel* level) override {
            LevelInfoLayer::levelDownloadFinished(level);

            if (m_songWidget->m_downloadBtn->isVisible())
                m_songWidget->m_downloadBtn->activate();
        }
    };
}