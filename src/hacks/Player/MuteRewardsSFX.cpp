#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/FMODAudioEngine.hpp>

namespace eclipse::hacks::Player {
    class $hack(MuteRewardsSFX) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.muterewardssfx")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Mute Rewards SFX on Death"; }
    };

    REGISTER_HACK(MuteRewardsSFX)

    constexpr std::array<std::string_view, 4> badSFX = {
        "achievement_01.ogg", "magicExplosion.ogg", "gold02.ogg", "secretKey.ogg"
    };

    class $modify(MuteRewardsSFXFMODAEHook, FMODAudioEngine) {
        static void onModify(auto& self) {
            FIRST_PRIORITY("FMODAudioEngine::playEffect");
            // change to FIRST_PRIORITY so ControlVanillaSFX (also by RayDeeUx) doesnt bork
            HOOKS_TOGGLE_ALL("player.muterewardssfx");
        }

        void playEffect(gd::string path, float speed, float p2, float volume) {
            auto* pl = utils::get<PlayLayer>();

            // play sfx if not in playlayer
            if (!pl || !pl->m_player1->m_isDead || pl->m_isPaused)
                return FMODAudioEngine::playEffect(path, speed, p2, volume);

            if (std::ranges::find(badSFX, std::string_view(path)) == badSFX.end())
                FMODAudioEngine::playEffect(path, speed, p2, volume);
        }
    };
}
