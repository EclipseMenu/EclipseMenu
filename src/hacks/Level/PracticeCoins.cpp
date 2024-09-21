#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class PracticeCoins : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Practice Coins", "level.practicecoins")
                ->setDescription("Allows you to collect coins in practice mode.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Practice Coins"; }
    };

    REGISTER_HACK(PracticeCoins)

    class $modify(PracticeCoinsBGLHook, GJBaseGameLayer) {
        void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* sectionObjects, int p2, float p3) {
            if (!m_isPracticeMode)
                return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, p2, p3);

            if (!config::get<bool>("level.practicecoins", false))
                return GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, p2, p3);

            auto playerRect = player->getObjectRect();
            for (auto obj : *sectionObjects) {
                // check if the object is a coin
                if (obj->m_objectType != GameObjectType::SecretCoin && obj->m_objectType != GameObjectType::UserCoin)
                    continue;

                auto* effectSprite = geode::cast::typeinfo_cast<EffectGameObject*>(obj);
                if (!effectSprite) // weird, but just in case
                    continue;

                // check if the coin was already collected
                if (effectSprite->getOpacity() == 0)
                    continue;

                // check if the player is colliding with the object
                auto objectRect = effectSprite->getObjectRect();
                if (!playerRect.intersectsRect(objectRect))
                    continue;

                // reconstruction of the original code
                effectSprite->EffectGameObject::triggerObject(this, player->m_uniqueID, nullptr); // not sure if needed
                GJBaseGameLayer::destroyObject(effectSprite); // plays the coin collect animation
                // if (!GJBaseGameLayer::hasUniqueCoin(effectSprite))
                //     GJBaseGameLayer::pickupItem(effectSprite); // not required, because the coin is not saved anyway
                GJBaseGameLayer::gameEventTriggered(GJGameEvent::UserCoin, 0, 0); // event triggers
            }

            GJBaseGameLayer::collisionCheckObjects(player, sectionObjects, p2, p3);
        }
    };
}