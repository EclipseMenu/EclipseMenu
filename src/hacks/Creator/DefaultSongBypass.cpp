#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/SongSelectNode.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>

namespace eclipse::hacks::Creator {

    class DefaultSongBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Default Song Bypass", "creator.defaultsongbypass")
                ->handleKeybinds()
                ->setDescription("Unlocks hidden songs in the level editor");
        }

        [[nodiscard]] const char* getId() const override { return "Default Song Bypass"; }
    };

    REGISTER_HACK(DefaultSongBypass)

    class $modify(DefaultSongBypassSSNHook, SongSelectNode) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.defaultsongbypass")

        void audioPrevious(cocos2d::CCObject* sender) {
            this->m_selectedSongID = std::max(0, this->m_selectedSongID - 1);
            this->updateAudioLabel();
        }

        void audioNext(cocos2d::CCObject* sender) {
            this->m_selectedSongID = std::max(0, this->m_selectedSongID + 1);
            this->updateAudioLabel();
        }
    };

    class $modify(DefaultSongBypassMSLHook, MoreSearchLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.defaultsongbypass")

        void audioPrevious(cocos2d::CCObject* sender) {
            auto song = GameLevelManager::get()->getIntForKey("song_filter");
            MoreSearchLayer::selectSong(std::max(1, song - 1));
        }

        void audioNext(cocos2d::CCObject* sender) {
            auto song = GameLevelManager::get()->getIntForKey("song_filter");
            MoreSearchLayer::selectSong(std::max(1, song + 1));
        }

        void selectSong(int songID) {
            songID = std::max(1, songID);
            GameLevelManager::get()->setIntForKey(songID, "song_filter");
            this->updateAudioLabel();
        }
    };

}
