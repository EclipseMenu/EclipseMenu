#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/gui/components/float-toggle.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCDelayTime.hpp>

namespace eclipse::hacks::Player {
    class $hack(RespawnDelay) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");

            config::setIfEmpty("player.respawndelay.toggle", false);
            config::setIfEmpty("player.respawndelay", 1.f);

            tab->addFloatToggle("player.respawndelay", 0.f, 120.f, "%.2f s.")->handleKeybinds();
        }

        [[nodiscard]] int32_t getPriority() const override { return 1; }
        [[nodiscard]] const char* getId() const override { return "Respawn Delay"; }
    };

    REGISTER_HACK(RespawnDelay)

    class $modify(RespawnDelayPLHook, PlayLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.respawndelay.toggle")

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            PlayLayer::destroyPlayer(player, object);

            auto delay = config::get<float>("player.respawndelay", 1.f);
            if (auto* respawnSequence = this->getActionByTag(0x10)) {
                // Recreate the sequence with the new delay
                this->stopAction(respawnSequence);
                auto* newSequence = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(delay),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                    nullptr
                );
                newSequence->setTag(0x10);
                this->runAction(newSequence);
            }
        }
    };
}
