#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/FMODAudioEngine.hpp>

namespace eclipse::hacks::Player {

    class MuteRewardsSFX : public hack::Hack {
        config::setIfEmpty<bool>("player.muterewardssfx", false);
        void init() override {
            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("Mute Rewards SFX on Death", "player.muterewardssfx")->setDescription("Disables player death effect")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Mute Rewards SFX on Death"; }
    };

    REGISTER_HACK(MuteRewardsSFX)

    class $modify(FMODAudioEngine) {
        struct Fields {
            const std::list<std::string> badSFX = { "achievement_01.ogg", "magicExplosion.ogg", "gold02.ogg", "secretKey.ogg" };
        };
        static void onModify(auto& self) {
            SAFE_PRIORITY("FMODAudioEngine::playEffect");
        }

        void playEffect(gd::string path, float p1, float p2, float p3) {
            auto pl = PlayLayer::get();
            if (!pl) { return FMODAudioEngine::sharedEngine()->playEffect(path, p1, p2, p3); } // play sfx if not in playlayer
            bool isPlayerAlive = !pl->m_player1->m_isDead;
            bool pauseLayerActive = cocos2d::CCDirector::get()->getRunningScene()->getChildByIDRecursive("PauseLayer");
            bool notFoundInBadSFX = std::find(m_fields->badSFX.begin(), m_fields->badSFX.end(), std::string(path)) == m_fields->badSFX.end();
            bool isSettingDisabled = !config::get<bool>("player.muterewardssfx");
            // these bools could be in one if statement but are separated for readability
            if (isSettingDisabled || pauseLayerActive || isPlayerAlive || notFoundInBadSFX) { FMODAudioEngine::sharedEngine()->playEffect(path, p1, p2, p3); }
        }
    };

}