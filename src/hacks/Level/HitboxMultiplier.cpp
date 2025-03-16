#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/OBB2D.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    static GameObject* s_rotatedGameObject = nullptr;
    static std::deque<GameObject*> s_affectedGameObjects;

    float getMultiplier(GameObject* gameObject) {
        if (!utils::get<GJBaseGameLayer>())
            return 1.f;

        if ((gameObject == utils::get<GJBaseGameLayer>()->m_player1 || gameObject == utils::get<GJBaseGameLayer>()->m_player2))
            return config::get<float>("level.hitbox_multiplier.player", 1.f);
        if (gameObject->m_objectType == GameObjectType::Hazard || gameObject->m_objectType == GameObjectType::AnimatedHazard)
            return config::get<float>("level.hitbox_multiplier.hazard", 1.f);
        if (gameObject->m_objectType == GameObjectType::Solid || gameObject->m_objectType == GameObjectType::Slope)
            return config::get<float>("level.hitbox_multiplier.solid", 1.f);

        return 1.f;
    }

    class $modify(HitboxGameObject, GameObject) {
        struct Fields {
            float m_originalRadius = -1.f;
        };

        ADD_HOOKS_DELEGATE("level.hitbox_multiplier")

        cocos2d::CCRect getObjectRect(float x, float y) {
            s_affectedGameObjects.push_back(this);

            if (m_fields->m_originalRadius < 0)
                m_fields->m_originalRadius = m_objectRadius;

            float multiplier = getMultiplier(this);
            m_objectRadius = m_fields->m_originalRadius * getMultiplier(this);
            return GameObject::getObjectRect(x * multiplier, y * multiplier);
        }

        void updateOrientedBox() {
            s_rotatedGameObject = this;
            GameObject::updateOrientedBox();
        }
    };

    void forceHitboxRecalculation() {
        for (GameObject* gameObject : s_affectedGameObjects) {
            gameObject->setObjectRectDirty(true);
            gameObject->setOrientedRectDirty(true);
            gameObject->m_objectRadius = static_cast<HitboxGameObject*>(gameObject)->m_fields->m_originalRadius;
        }

        s_affectedGameObjects.clear();
    }

    void reset() {
        s_affectedGameObjects.clear();
        s_rotatedGameObject = nullptr;
    }

    class $hack(HitboxMultiplier) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.hitbox_multiplier", false);
            config::setIfEmpty("level.hitbox_multiplier.player", 1.f);
            config::setIfEmpty("level.hitbox_multiplier.solid", 1.f);
            config::setIfEmpty("level.hitbox_multiplier.hazard", 1.f);

            tab->addToggle("level.hitbox_multiplier")->handleKeybinds()->setDescription()
               ->callback([](bool) { forceHitboxRecalculation(); })
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addInputFloat("level.hitbox_multiplier.player", 0.f, 10.f, "%.2fx");
                   options->addInputFloat("level.hitbox_multiplier.solid", 0.f, 10.f, "%.2fx");
                   options->addInputFloat("level.hitbox_multiplier.hazard", 0.f, 10.f, "%.2fx");
               });
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"level.hitbox_multiplier", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Hitbox Multiplier"; }
    };

    REGISTER_HACK(HitboxMultiplier)

    class $modify(HitboxMultiplierPLHook, PlayLayer) {
        bool init(GJGameLevel* level, bool unk, bool dontCreateObjects) {
            reset();
            return PlayLayer::init(level, unk, dontCreateObjects);
        }

        void resetLevel() {
            forceHitboxRecalculation();
            PlayLayer::resetLevel();
        }

        void onQuit() {
            reset();
            PlayLayer::onQuit();
        }
    };

    class $modify(HitboxMultiplierLELHook, LevelEditorLayer) {
        bool init(GJGameLevel* level, bool unk) {
            reset();
            bool res = LevelEditorLayer::init(level, unk);
            reset();
            return res;
        }

        void onPlaytest() {
            forceHitboxRecalculation();
            LevelEditorLayer::onPlaytest();
        }
    };

    class $modify(HitboxMultiplierOBB2DHook, OBB2D) {
        void calculateWithCenter(cocos2d::CCPoint center, float width, float heigth, float rotation) {
            if (!s_rotatedGameObject || !utils::get<GJBaseGameLayer>() || !config::get<bool>("level.hitbox_multiplier", false)) {
                s_rotatedGameObject = nullptr;
                return OBB2D::calculateWithCenter(center, width, heigth, rotation);
            }

            s_affectedGameObjects.push_back(s_rotatedGameObject);

            float multiplier = getMultiplier(s_rotatedGameObject);
            OBB2D::calculateWithCenter(center, width * multiplier, heigth * multiplier, rotation);

            s_rotatedGameObject = nullptr;
        }
    };
};
