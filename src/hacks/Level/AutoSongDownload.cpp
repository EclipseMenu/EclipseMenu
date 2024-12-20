#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoSongDownload : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.autosongdownload")
                ->handleKeybinds()
                ->setDescription();
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