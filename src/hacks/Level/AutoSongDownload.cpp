#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoSongDownload : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Auto Song Download", "level.autosongdownload")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Auto Song Download"; }
    };

    REGISTER_HACK(AutoSongDownload)

    class $modify(AutoSongDownloadLILHook, LevelInfoLayer) {
        void levelDownloadFinished(GJGameLevel* level) override {
            LevelInfoLayer::levelDownloadFinished(level);

            if (m_songWidget->m_downloadBtn->isVisible() && config::get<bool>("level.autosongdownload", false))
                m_songWidget->m_downloadBtn->activate();
        }
    };
}