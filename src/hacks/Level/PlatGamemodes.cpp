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

    class $modify(GJBaseGameLayer) { // i took this from qolmod hehehe >:D
        void collisionCheckObjects(PlayerObject* p0, gd::vector<GameObject*>* p1, int p2, float p3) {
            if (!config::get<bool>("level.platgamemodes", false)) return GJBaseGameLayer::collisionCheckObjects(p0, p1, p2, p3);

            for (size_t i = 0; i < p1->size(); i++) {
                auto obj = p1->at(i);

                if (p0->getObjectRect().intersectsRect(obj->getObjectRect())) {
                    if (obj->m_objectType == GameObjectType::WavePortal || obj->m_objectType == GameObjectType::SwingPortal) {
                        if(this->canBeActivatedByPlayer(p0, geode::cast::as<EffectGameObject*>(obj))) {
                            this->playerWillSwitchMode(p0, obj);
                            this->switchToFlyMode(p0, obj, false, geode::cast::as<int>(obj->m_objectType));                        
                            obj->playShineEffect();
                        }
                    }
                }
            }

            GJBaseGameLayer::collisionCheckObjects(p0, p1, p2, p3);
        }
    };
}