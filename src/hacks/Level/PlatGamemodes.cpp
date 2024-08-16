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

    class $modify(PlatGamemodesBGLHook, GJBaseGameLayer) {
        void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* sectionObjects, int p2, float p3) {
            if (!m_isPlatformer)
                return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, p2, p3);

            if (!config::get<bool>("level.platgamemodes", false))
                return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, p2, p3);

            auto playerRect = player->getObjectRect();
            for (auto obj : *sectionObjects) {
                // check if the object is a coin
                if (obj->m_objectType != GameObjectType::WavePortal && obj->m_objectType != GameObjectType::SwingPortal)
                    continue;

                // check if the player is colliding with the object
                auto objectRect = obj->getObjectRect();
                if (!playerRect.intersectsRect(objectRect))
                    continue;

                // reconstruction of the original code
                if (!this->canBeActivatedByPlayer(player, static_cast<EffectGameObject*>(obj)))
                    continue;

                GJBaseGameLayer::playerWillSwitchMode(player, obj);
                player->switchedToMode(obj->m_objectType);

                player->m_lastPortalPos = obj->getPosition();
                player->m_lastActivatedPortal = obj;
                auto* portalObj = obj;
                if (m_gameState.m_isDualMode && m_gameState.m_unkGameObjPtr2)
                    portalObj = m_gameState.m_unkGameObjPtr2;
                m_gameState.m_unkGameObjPtr1 = portalObj;

                if (obj->m_objectType == GameObjectType::WavePortal)
                    player->toggleDartMode(true, portalObj->m_hasNoEffects);
                else if (obj->m_objectType == GameObjectType::SwingPortal)
                    player->toggleSwingMode(true, portalObj->m_hasNoEffects);

                if (auto* pl = geode::cast::typeinfo_cast<PlayLayer*>(this))
                    pl->PlayLayer::toggleGlitter(true);
                obj->playShineEffect();

                // FIXME: find bindings
                // if (auto* enh = geode::cast::typeinfo_cast<EnhancedGameObject*>(obj))
                //     enh->EnhancedGameObject::hasBeenActivatedByPlayer(player);
            }

            GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, p2, p3);
        }
    };
}