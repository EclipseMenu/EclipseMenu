#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    static bool s_isDead = false;
    class $hack(WaveTrailOnDeath) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.wavetrailondeath")->setDescription()->handleKeybinds()->addOptions([](auto options) {
                options->addToggle("player.wavetrailondeath.persist-trail")->setDescription()->handleKeybinds();
            });
        }
        [[nodiscard]] const char* getId() const override { return "Wave Trail on Death"; }
    };

    REGISTER_HACK(WaveTrailOnDeath)

    class $modify(WaveTrailOnDeathPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.wavetrailondeath")

        void playerDestroyed(bool p0) {
            if (auto* gjbgl = m_gameLayer)
                s_isDead = this == gjbgl->m_player1 || this == gjbgl->m_player2;

            PlayerObject::playerDestroyed(p0);
            m_waveTrail->stopActionByTag(8);

            s_isDead = false;
        }

        #ifdef GEODE_IS_MACOS
        void deactivateStreak(bool stop) {
            PlayerObject::deactivateStreak(stop);
            if (s_isDead || config::get<"player.wavetrailondeath.persist-trail", bool>(false)) return;
                m_waveTrail->stopActionByTag(8);
        }
        #endif

        void fadeOutStreak2(float duration) {
            if (s_isDead || config::get<"player.wavetrailondeath.persist-trail", bool>(false)) return;
            PlayerObject::fadeOutStreak2(duration);
        }

        void createFadeOutDartStreak() {
            if (s_isDead || config::get<"player.wavetrailondeath.persist-trail", bool>(false)) return;
            PlayerObject::createFadeOutDartStreak();
        }
    };
}
