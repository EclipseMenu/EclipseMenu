#include <Core/Module.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace eclipse::prelude;

$hack(Noclip, "player.noclip") {
    .tab = MenuTab::Player,
    .type = ModuleType::Toggle,
    .cheat = true,
};

class $modify(NoclipPL, PlayLayer) {
    BIND_MODULE_HIGHPRIO(Noclip);

    void destroyPlayer(PlayerObject* player, GameObject* object) override {
        if (object == m_anticheatSpike) {
            return PlayLayer::destroyPlayer(player, object);
        }
    }
};