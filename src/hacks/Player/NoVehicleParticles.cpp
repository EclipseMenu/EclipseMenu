#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {
    void onHideParticles(bool state) {
        auto change_particles_state = [](PlayerObject* player, bool toggle) {
            player->m_playerGroundParticles->setVisible(toggle);
            player->m_ufoClickParticles->setVisible(toggle);
            player->m_dashParticles->setVisible(toggle);
            player->m_robotBurstParticles->setVisible(toggle);
            player->m_trailingParticles->setVisible(toggle);
            player->m_shipClickParticles->setVisible(toggle);
            player->m_vehicleGroundParticles->setVisible(toggle);
            player->m_landParticles0->setVisible(toggle);
            player->m_landParticles1->setVisible(toggle);
            player->m_swingBurstParticles1->setVisible(toggle);
            player->m_swingBurstParticles2->setVisible(toggle);
        };

        auto* bgl = utils::get<GJBaseGameLayer>();

        if (!bgl) return;

        change_particles_state(bgl->m_player1, !state);
        change_particles_state(bgl->m_player2, !state);
    }

    class $hack(NoParticles) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");

            tab->addToggle("player.novehicleparticles")
               ->handleKeybinds()
               ->setDescription()
               ->callback(onHideParticles);
        }

        [[nodiscard]] const char* getId() const override { return "No Vehicle Particles"; }
    };

    REGISTER_HACK(NoParticles)

    class $modify(NoParticlesBGLHook, GJBaseGameLayer) {
        void createPlayer() {
            GJBaseGameLayer::createPlayer();

            onHideParticles(config::get<bool>("player.novehicleparticles", false));
        }
    };
}
