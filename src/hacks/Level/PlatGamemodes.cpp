#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class PlatGamemodes : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("All Modes in Platformer", "level.platgamemodes")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "All Modes in Platformer"; }
    };

    REGISTER_HACK(PlatGamemodes)

    class $modify(GJBaseGameLayer) {
        void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* gameObjects, int p2, float p3) {
            if (!config::get<bool>("level.platgamemodes", false))
                return GJBaseGameLayer::collisionCheckObjects(player, gameObjects, p2, p3);

            bool isPlatformer = m_isPlatformer;
            m_isPlatformer = false;
            GJBaseGameLayer::collisionCheckObjects(player, gameObjects, p2, p3);
            m_isPlatformer = isPlatformer;
        }
    };
}