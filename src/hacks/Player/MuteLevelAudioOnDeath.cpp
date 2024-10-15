#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EffectGameObject.hpp>

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

    class $modify(MuteLevelAudioOnDeathEGOHook, EffectGameObject) {
        ADD_HOOKS_DELEGATE("player.mutelevelaudioondeath")
        void triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
            PlayLayer* pl = PlayLayer::get();

            // do nothing if playlayer is nullptr
            if (!pl) return EffectGameObject::triggerObject(p0, p1, p2);

            PlayerObject* player = pl->m_player1;

            // do nothing if player is nullptr
            if (!player || !player->m_isDead) return EffectGameObject::triggerObject(p0, p1, p2);

            int id = this->m_objectID;

            if (player->m_isDead && id != 3602 && id != 1934) return EffectGameObject::triggerObject(p0, p1, p2);
        }
    };

}
