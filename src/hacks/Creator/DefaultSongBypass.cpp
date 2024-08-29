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
        static void onModify(auto& self) {
            SAFE_PRIORITY("SongSelectNode::audioPrevious");
            SAFE_PRIORITY("SongSelectNode::audioNext");
        }

        void audioPrevious(cocos2d::CCObject* sender) {
            if (!config::get<bool>("creator.defaultsongbypass", false))
                return SongSelectNode::audioPrevious(sender);

            this->m_selectedSongID = std::max(0, this->m_selectedSongID - 1);
            SongSelectNode::updateAudioLabel();
        }

        void audioNext(cocos2d::CCObject* sender) {
            if (!config::get<bool>("creator.defaultsongbypass", false))
                return SongSelectNode::audioNext(sender);

            this->m_selectedSongID = std::max(0, this->m_selectedSongID + 1);
            SongSelectNode::updateAudioLabel();
        }
    };

    class $modify(DefaultSongBypassMSLHook, MoreSearchLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("MoreSearchLayer::audioPrevious");
            SAFE_PRIORITY("MoreSearchLayer::audioNext");
            SAFE_PRIORITY("MoreSearchLayer::selectSong");
        }

        void audioPrevious(cocos2d::CCObject* sender) {
            if (!config::get<bool>("creator.defaultsongbypass", false))
                return MoreSearchLayer::audioPrevious(sender);

            auto song = GameLevelManager::get()->getIntForKey("song_filter");
            MoreSearchLayer::selectSong(std::max(1, song - 1));
        }

        void audioNext(cocos2d::CCObject* sender) {
            if (!config::get<bool>("creator.defaultsongbypass", false))
                return MoreSearchLayer::audioNext(sender);

            auto song = GameLevelManager::get()->getIntForKey("song_filter");
            MoreSearchLayer::selectSong(std::max(1, song + 1));
        }

        void selectSong(int songID) {
            if (!config::get<bool>("creator.defaultsongbypass", false))
                return MoreSearchLayer::selectSong(songID);

            songID = std::max(1, songID);
            GameLevelManager::get()->setIntForKey(songID, "song_filter");
            MoreSearchLayer::updateAudioLabel();
        }
    };

}
