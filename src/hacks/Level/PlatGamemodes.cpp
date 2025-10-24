#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(PlatGamemodes) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.platgamemodes")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "All Modes in Platformer"; }
    };

    REGISTER_HACK(PlatGamemodes)

    class $modify(PlatGamemodesBGLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("level.platgamemodes")

        void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* sectionObjects, int objectCount, float dt) {
            if (!m_isPlatformer)
                return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, objectCount, dt);

            auto playerRect = player->getObjectRect();
            for (int i = 0; i < objectCount; i++) {
                auto obj = sectionObjects->at(i);

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
                if (m_gameState.m_isDualMode && m_gameState.m_lastActivatedPortal2)
                    portalObj = m_gameState.m_lastActivatedPortal2;
                m_gameState.m_lastActivatedPortal1 = portalObj;

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

            GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, objectCount, dt);
        }
    };
}
