#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {
    class TrajectorySimulation {
    private:
        PlayerObject* m_player1{};
        PlayerObject* m_player2{};

        bool m_simulating = false;
        bool m_simulationDead = false;
        bool m_player1Pressed = false;
        bool m_player2Pressed = false;

        float m_frameDt = 0.f;

    public:
        cocos2d::CCDrawNode* getDrawNode() const {
            class TrajectoryDrawNode : public cocos2d::CCDrawNode {
            public:
                static TrajectoryDrawNode* create() {
                    auto ret = new TrajectoryDrawNode();
                    if (ret->init()) {
                        ret->autorelease();
                        ret->m_bUseArea = false;
                        return ret;
                    }
                    delete ret;
                    return nullptr;
                }
            };

            static TrajectoryDrawNode* instance = nullptr;

            if (!instance) {
                instance = TrajectoryDrawNode::create();
                instance->retain();
                instance->setID("show-trajectory-draw-node"_spr);
            }

            return instance;
        }

    private:
        void drawRectangleHitbox(const cocos2d::CCRect& rect, const gui::Color& color, const gui::Color& borderColor) const {
            std::array vertices = {
                cocos2d::CCPoint{rect.getMinX(), rect.getMinY()},
                cocos2d::CCPoint{rect.getMinX(), rect.getMaxY()},
                cocos2d::CCPoint{rect.getMaxX(), rect.getMaxY()},
                cocos2d::CCPoint{rect.getMaxX(), rect.getMinY()}
            };

            getDrawNode()->drawPolygon(
                vertices.data(), vertices.size(),
                {color.r, color.g, color.b, 0},
                0.25f, borderColor
            );
        }

        void drawForPlayer(PlayerObject* player) const {
            cocos2d::CCRect rect1 = player->getObjectRect();
            cocos2d::CCRect rect2 = player->getObjectRect(0.25f, 0.25f);

            auto color = config::get<gui::Color>("level.showhitboxes.player_color", {1.f, 0, 0, 1.f});
            auto colorInner = config::get<gui::Color>("level.showhitboxes.player_color_inner", {0, 1.f, 0, 1.f});

            drawRectangleHitbox(rect1, color, color);
            drawRectangleHitbox(rect2, colorInner, colorInner);
        }

        void resetCollisionLog(PlayerObject* self) const {
            self->m_collisionLogTop->removeAllObjects();
            self->m_collisionLogBottom->removeAllObjects();
            self->m_collisionLogLeft->removeAllObjects();
            self->m_collisionLogRight->removeAllObjects();
        }

        void iterateForPlayer(PlayerObject* player, bool down, bool isPlayer2) {
            player->setVisible(false);
            m_simulationDead = false;

            bool iterationActionDone = false;

            const size_t iterations = config::get<int>("player.showtrajectory.iterations", 300);
            auto pl = utils::get<PlayLayer>();

            for (size_t i = 0; i < iterations; i++) {
                cocos2d::CCPoint initialPlayerPosition = player->getPosition();
                resetCollisionLog(player);

                pl->checkCollisions(player, m_frameDt, false);

                // ReSharper disable once CppDFAConstantConditions
                if (m_simulationDead)
                    break;

                if (!iterationActionDone && (!isPlayer2 && !m_player1Pressed || isPlayer2 && !m_player2Pressed)) {
                    iterationActionDone = true;
                    down ? player->pushButton(PlayerButton::Jump) : player->releaseButton(PlayerButton::Jump);
                }

                player->update(m_frameDt);
                //player->updateSpecial(m_frameDt);
                //player->updateRotation(m_frameDt);

                getDrawNode()->drawSegment(
                    initialPlayerPosition,
                    player->getPosition(), 0.65f,
                    down ? cocos2d::ccColor4F{0.f, 1.f, 0.1f, 1.f} : cocos2d::ccColor4F{1.f, 0.f, 0.1f, 1.f}
                );
            }

            drawForPlayer(player);
        }

        void buttonForPlayer(PlayerObject* player, PlayerObject* playerBase, bool down) {
            if (!player || !playerBase) return;

            bool isPlayer2 = playerBase == utils::get<PlayLayer>()->m_player2;

            player->copyAttributes(playerBase);
            player->m_gravityMod = playerBase->m_gravityMod;

            if ((isPlayer2 && m_player2Pressed) || (!isPlayer2 && m_player1Pressed)) {
                down ? player->pushButton(PlayerButton::Jump) : player->releaseButton(PlayerButton::Jump);
            }

            player->m_isOnGround = playerBase->m_isOnGround;

            iterateForPlayer(player, down, isPlayer2);
        }

        void simulateForPlayer(PlayerObject* player, PlayerObject* player2, PlayerObject* playerBase) {
            buttonForPlayer(player, playerBase, true);
            buttonForPlayer(player2, playerBase, false);
        }

        PlayerObject* createTrajectoryPlayer() const {
            PlayLayer* pl = utils::get<PlayLayer>();

            PlayerObject* player = PlayerObject::create(1, 1, pl, pl, true);
            // player->retain();
            player->setPosition({0, 105});
            player->setVisible(false);
            player->setID("show-trajectory-player"_spr);
            pl->m_objectLayer->addChild(player);

            return player;
        }

    public:
        void init() {
            PlayLayer* pl = utils::get<PlayLayer>();

            m_player1 = createTrajectoryPlayer();
            m_player2 = createTrajectoryPlayer();

            pl->m_debugDrawNode->getParent()->addChild(getDrawNode());
            getDrawNode()->setZOrder(pl->m_debugDrawNode->getZOrder());
        }

        void quit() {
            m_player1 = nullptr;
            m_player2 = nullptr;
        }

        bool isSimulating() const {
            return m_simulating;
        }

        bool isSimulationDead(PlayerObject* player) {
            if (m_simulating && (player == m_player1 || player == m_player2)) {
                m_simulationDead = true;
                return true;
            }

            return false;
        }

        void handleButton(bool down, bool player1) {
            if (player1)
                m_player1Pressed = down;
            else
                m_player2Pressed = down;
        }

        void simulate() {
            PlayLayer* pl = utils::get<PlayLayer>();

            if (!pl) return;

            m_simulating = true;

            getDrawNode()->clear();

            simulateForPlayer(m_player1, m_player2, pl->m_player1);

            if (pl->m_gameState.m_isDualMode)
                simulateForPlayer(m_player2, m_player1, pl->m_player2);

            m_simulating = false;
        }

        void hide() const {
            getDrawNode()->setVisible(false);
        }

        void setFrameDelta(float dt) {
            PlayLayer* pl = utils::get<PlayLayer>();
            m_frameDt = dt / pl->m_gameState.m_timeWarp;
        }
    };

    static TrajectorySimulation s_simulation;

    class $hack(ShowTrajectory) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            auto toggle = tab->addToggle("player.showtrajectory")->setDescription()->handleKeybinds();

            config::setIfEmpty("player.showtrajectory.iterations", 300);

            toggle->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                options->addInputInt("player.showtrajectory.iterations", 1, 1000);
            });

            config::addDelegate("player.showtrajectory", [] {
                auto value = config::get<bool>("player.showtrajectory", false);
                s_simulation.getDrawNode()->clear();
                s_simulation.getDrawNode()->setVisible(value);
            });
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"player.showtrajectory", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Show Trajectory"; }
    };

    REGISTER_HACK(ShowTrajectory)

    class $modify(ShowTrajectoryPLHook, PlayLayer) {
        static void onModify(auto& self) {
            HOOKS_TOGGLE(
                "player.showtrajectory", PlayLayer,
                "destroyPlayer",
                "playEndAnimationToPos"
            );
        }

        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

            s_simulation.init();

            return true;
        }

        void destroyPlayer(PlayerObject* player, GameObject* gameObject) override {
            if (gameObject != m_anticheatSpike && s_simulation.isSimulationDead(player)) return;

            PlayLayer::destroyPlayer(player, gameObject);
        }

        void playEndAnimationToPos(cocos2d::CCPoint p0) {
            if (s_simulation.isSimulating()) return;

            PlayLayer::playEndAnimationToPos(p0);
        }

        void onQuit() {
            PlayLayer::onQuit();
            s_simulation.quit();
        }
    };

    class $modify(ShowTrajectoryLELHook, LevelEditorLayer) {
        bool init(GJGameLevel* level, bool unk) {
            bool result = LevelEditorLayer::init(level, unk);
            s_simulation.quit();
            return result;
        }
    };

    class $modify(ShowTrajectoryHSHook, HardStreak) {
        ADD_HOOKS_DELEGATE("player.showtrajectory")

        void addPoint(cocos2d::CCPoint p0) {
            if (s_simulation.isSimulating()) return;

            HardStreak::addPoint(p0);
        }
    };

    class $modify(ShowTrajectoryEGOHook, EffectGameObject) {
        ADD_HOOKS_DELEGATE("player.showtrajectory")

        void triggerObject(GJBaseGameLayer* p0, int p1, const gd::vector<int>* p2) override {
            if (s_simulation.isSimulating()) return;

            return EffectGameObject::triggerObject(p0, p1, p2);
        }
    };

    class $modify(ShowTrajectoryGOHook, GameObject) {
        ADD_HOOKS_DELEGATE("player.showtrajectory")

        void playShineEffect() {
            if (s_simulation.isSimulating()) return;

            GameObject::playShineEffect();
        }
    };

    class $modify(ShowTrajectoryPOHook, PlayerObject) {
        ADD_HOOKS_DELEGATE("player.showtrajectory")

        void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to) {
            if (s_simulation.isSimulating()) return;

            PlayerObject::playSpiderDashEffect(from, to);
        }

        void incrementJumps() {
            if (s_simulation.isSimulating()) return;

            PlayerObject::incrementJumps();
        }

        void update(float dt) override {
            PlayerObject::update(dt);

            if (utils::get<PlayLayer>() && !s_simulation.isSimulating())
                s_simulation.setFrameDelta(dt);
        }

        void ringJump(RingObject* p0, bool p1) {
            if (s_simulation.isSimulating()) return;

            PlayerObject::ringJump(p0, p1);
        }
    };

    class $modify(ShowTrajectoryBGLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("player.showtrajectory")

        bool canBeActivatedByPlayer(PlayerObject* p0, EffectGameObject* p1) {
            if (s_simulation.isSimulating())
                return false;

            return GJBaseGameLayer::canBeActivatedByPlayer(p0, p1);
        }

        void handleButton(bool down, int button, bool isPlayer1) {
            if (button == 1)
                s_simulation.handleButton(down, isPlayer1);

            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }

        void flipGravity(PlayerObject* p0, bool p1, bool p2) {
            if (s_simulation.isSimulating()) return;

            GJBaseGameLayer::flipGravity(p0, p1, p2);
        }

        //don't trigger objects early
        void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* vec, int objectsCount, float dt) {
            if (s_simulation.isSimulating()) {
                gd::vector<GameObject*> extra;
                #ifndef GEODE_IS_ANDROID // vector::reserve is not available on Android
                extra.reserve(objectsCount);
                #endif
                for (int i = 0; i < objectsCount; i++) {
                    GameObject* obj = vec->at(i);
                    if (obj->m_objectType == GameObjectType::Solid ||
                        obj->m_objectType == GameObjectType::Hazard ||
                        obj->m_objectType == GameObjectType::AnimatedHazard ||
                        obj->m_objectType == GameObjectType::Slope) {
                        extra.push_back(obj);
                    }
                }

                GJBaseGameLayer::collisionCheckObjects(player, &extra, extra.size(), dt);
                return;
            }

            GJBaseGameLayer::collisionCheckObjects(player, vec, objectsCount, dt);
        }

        void playerTouchedRing(PlayerObject* player, RingObject* ring) {
            if (s_simulation.isSimulating()) return;

            GJBaseGameLayer::playerTouchedRing(player, ring);
        }

        void updateCamera(float dt) {
            s_simulation.simulate();

            GJBaseGameLayer::updateCamera(dt);
        }
    };
}
