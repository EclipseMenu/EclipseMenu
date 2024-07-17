#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoSongDownload : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Automatic Song Download", "level.autosongdownload")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Automatic Song Download"; }
    };

    REGISTER_HACK(AutoSongDownload)

    class $modify(LevelInfoLayer) {
        void levelDownloadFinished(GJGameLevel* level) {
            LevelInfoLayer::levelDownloadFinished(level);

            if (m_songWidget->m_downloadBtn->isVisible() && config::get<bool>("level.autosongdownload", false)) {
                m_songWidget->m_downloadBtn->activate();
            }
        }
    };
}