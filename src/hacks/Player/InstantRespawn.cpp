#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {

    class InstantRespawn : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");
            tab->addToggle("Instant Respawn", "player.instantrespawn");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Instant Respawn"; }
    };

    REGISTER_HACK(InstantRespawn)

    class $modify(PlayLayer) {
        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            PlayLayer::destroyPlayer(player, object);
            if (config::get<bool>("player.instantrespawn", false)) {
                PlayLayer::delayedResetLevel();
            }
        }
    };

}
