#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {

    class MuteLevelAudioOnDeath : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("Mute Level Audio On Death", "player.mutelevelaudioondeath")
                ->setDescription("Mutes all level audio (Music + SFX) on player death. (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Mute Level Audio On Death"; }
    };

    REGISTER_HACK(MuteLevelAudioOnDeath)

    class $modify(MyPlayerObject, PlayerObject) {
        /*
        originally from erysedits by raydeeux.
        adapted by raydeeux, and improved using 2.206's bindings.
        -- raydeeux
        */
        void playerDestroyed(bool p0) {
            PlayLayer* pl = PlayLayer::get();

            // do nothing if playlayer is nullptr
            if (!pl) return PlayerObject::playerDestroyed(p0);

            if (this != pl->m_player1 && this != pl->m_player2)
                // avoid accidental sfx muting with deaths from others in globed
                return PlayerObject::playerDestroyed(p0);

            if (config::get<bool>("player.mutelevelaudioondeath", false)) {

                if (pl->m_isPracticeMode && !pl->m_practiceMusicSync)
                    // do nothing if in practice mode BUT practice sync disabled
                    return PlayerObject::playerDestroyed(p0);

                const auto fmod = FMODAudioEngine::sharedEngine();

                /*
                stopAllMusic(), while not inlined, does not represent
                accurate behavior, and forces music to restart
                in platformer levels.
                gd handles restarting music in classic levels.
                */
                fmod->pauseAllMusic();

                if (this == pl->m_player2 && pl->m_level->m_twoPlayerMode)
                    /*
                    avoid stopping sfx twice -- thank you clicksounds.
                    this is here in case adam729 ports death sound randomizer,
                    but more importantly because we want to hear player death sfx.
                    */
                    return PlayerObject::playerDestroyed(p0);

                /*
                originally the function call was stopAllEffects()
                which should've sufficed for 90% of SFX,
                but accuracy > getting the job done in case there's
                SFX that lasts far, FAR longer.
                */
                fmod->pauseAllEffects();
            }
            PlayerObject::playerDestroyed(p0);
        }
    };

}