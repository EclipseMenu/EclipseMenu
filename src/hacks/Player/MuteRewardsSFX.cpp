#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/FMODAudioEngine.hpp>

namespace eclipse::hacks::Player {

    class MuteRewardsSFX : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Mute Rewards SFX on Death", "player.muterewardssfx")
                ->setDescription("Disables all rewards SFX (orbs, diamonds, keys) when the player dies. (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Mute Rewards SFX on Death"; }
    };

    REGISTER_HACK(MuteRewardsSFX)

    const std::array<std::string_view, 4> badSFX = { "achievement_01.ogg", "magicExplosion.ogg", "gold02.ogg", "secretKey.ogg" };
    class $modify(FMODAudioEngine) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("FMODAudioEngine::playEffect");
        }

        void playEffect(gd::string path, float p1, float p2, float p3) {
            if (!config::get<bool>("player.muterewardssfx", false))
                return FMODAudioEngine::sharedEngine()->playEffect(path, p1, p2, p3);

            auto pl = PlayLayer::get();

            // play sfx if not in playlayer
            if (!pl || !pl->m_player1->m_isDead || pl->m_isPaused)
                return FMODAudioEngine::sharedEngine()->playEffect(path, p1, p2, p3);

            // these bools could be in one if statement but are separated for readability
            bool notFoundInBadSFX = std::find(
                badSFX.begin(),
                badSFX.end(),
                std::string_view(path)
            ) == badSFX.end();

            if (notFoundInBadSFX)
                FMODAudioEngine::sharedEngine()->playEffect(path, p1, p2, p3);
        }
    };

}