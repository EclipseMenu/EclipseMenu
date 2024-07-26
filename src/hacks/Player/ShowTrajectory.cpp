#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/EnhancedGameObject.hpp>
#include <Geode/modify/HardStreak.hpp>

namespace eclipse::hacks::Player {
    class ShowTrajectory : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            tab->addToggle("Show Trajectory", "player.showtrajectory")
                ->setDescription("Shows where the player will be if they click/don't click.")
                ->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.showtrajectory", false); }
        [[nodiscard]] const char* getId() const override { return "Show Trajectory"; }
    };

    REGISTER_HACK(ShowTrajectory)

    class TrajectorySimulation {
    private:
        PlayerObject* m_player1;
        PlayerObject* m_player2;

        bool m_simulating = false;
        bool m_simuationDead = false;
        bool m_player1Pressed = false;
        bool m_player2Pressed = false;

        float m_frameDt = 0.f;
    private:
        cocos2d::CCDrawNode* getDrawNode() {

            class TrajectoryDrawNode : public cocos2d::CCDrawNode {
            public:
                static TrajectoryDrawNode* create() {
                    TrajectoryDrawNode* ret = new TrajectoryDrawNode();

                    if (ret && ret->init()) {
                        ret->autorelease();
                        ret->m_bUseArea = false;
                    }
                    else
                        CC_SAFE_DELETE(ret);
                    
                    return ret;
                }
            };

            static TrajectoryDrawNode* instance = nullptr;
            
            if (!instance) {
                instance = TrajectoryDrawNode::create();
                instance->retain();
            }

            return instance;
        }

        void drawRectangleHitbox(const cocos2d::CCRect& rect, cocos2d::ccColor4B colBase, cocos2d::ccColor4B colBorder) {
            const size_t N = 4;
            std::vector<cocos2d::CCPoint> points(N);

            points[0] = cocos2d::CCPointMake(rect.getMinX(), rect.getMinY());
            points[1] = cocos2d::CCPointMake(rect.getMinX(), rect.getMaxY());
            points[2] = cocos2d::CCPointMake(rect.getMaxX(), rect.getMaxY());
            points[3] = cocos2d::CCPointMake(rect.getMaxX(), rect.getMinY());

            getDrawNode()->drawPolygon(const_cast<cocos2d::CCPoint*>(points.data()), points.size(), ccc4FFromccc4B(colBase),
                                0.25, ccc4FFromccc4B(colBorder));
        }

        void drawForPlayer(PlayerObject* player) {
            if (!player) return;

            cocos2d::CCRect rect1 = player->getObjectRect();
            cocos2d::CCRect rect2 = player->getObjectRect(0.25f, 0.25f);
            drawRectangleHitbox(rect1, { 255, 0, 0, 0 }, { 255, 0, 0, 255 });
            drawRectangleHitbox(rect2, { 0, 255, 0, 0 }, { 0, 255, 0, 255 });
        }

        void resetCollisionLog(PlayerObject* self) {
            self->m_unk4e8->removeAllObjects();
            self->m_unk4ec->removeAllObjects();
            self->m_unk4f0->removeAllObjects();
            self->m_unk4f4->removeAllObjects();
        }

        void iterateForPlayer(PlayerObject* player, bool down, bool isPlayer2) {
            player->setVisible(false);
            m_simuationDead = false;

            bool iterationActionDone = false;

            const size_t iterations = 300;

            for (size_t i = 0; i < iterations; i++) {
                cocos2d::CCPoint initialPlayerPosition = player->getPosition();
                resetCollisionLog(player);

                PlayLayer::get()->checkCollisions(player, m_frameDt, false);

                if (m_simuationDead)
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
                    down ? cocos2d::ccColor4F{ 0.f, 1.f, 0.1f, 1.f } : cocos2d::ccColor4F{ 1.f, 0.f, 0.1f, 1.f }
                );
            }
            
            drawForPlayer(player);
        }

        void buttonForPlayer(PlayerObject* player, PlayerObject* playerBase, bool down) {
            if (!player || !playerBase) return;

            bool isPlayer2 = playerBase == PlayLayer::get()->m_player2;

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

        PlayerObject* createTrajectoryPlayer() {
            PlayLayer* pl = PlayLayer::get();

            PlayerObject* player = PlayerObject::create(1, 1, pl, pl, true);
            player->retain();
            player->setPosition({0, 105});
            player->setVisible(false);
            pl->m_objectLayer->addChild(player);

            return player;
        }

    public:
        void init() {
            PlayLayer* pl = PlayLayer::get();

            m_player1 = createTrajectoryPlayer();
            m_player2 = createTrajectoryPlayer();

            pl->m_debugDrawNode->getParent()->addChild(getDrawNode());
            getDrawNode()->setZOrder(pl->m_debugDrawNode->getZOrder());
        }

        void quit() {
            m_player1 = nullptr;
            m_player2 = nullptr;
        }

        bool isSimulating() {
            return m_simulating;
        }

        bool isSimulationDead(PlayerObject* player) {
            if (m_simulating && (player == m_player1 || player == m_player2)) {
                m_simuationDead = true;
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
            PlayLayer* pl = PlayLayer::get();

            if (!pl) return;

            m_simulating = true;

            getDrawNode()->setVisible(true);
            getDrawNode()->clear();

            simulateForPlayer(m_player1, m_player2, pl->m_player1);

            if (pl->m_gameState.m_isDualMode)
                simulateForPlayer(m_player2, m_player1, pl->m_player2);

            m_simulating = false;
        }

        void hide() {
            getDrawNode()->setVisible(false);
        }

        void setFrameDelta(float dt) {
            PlayLayer* pl = PlayLayer::get();
            m_frameDt = dt / pl->m_gameState.m_timeWarp;
        }
    };

    static TrajectorySimulation s_simulation;

    class $modify(PlayLayer) {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            bool result = PlayLayer::init(level, useReplay, dontCreateObjects);

            s_simulation.init();

            return result;
        }

        void destroyPlayer(PlayerObject* player, GameObject* gameObject) {
            if (s_simulation.isSimulationDead(player)) return;

            PlayLayer::destroyPlayer(player, gameObject);
        }

        void playEndAnimationToPos(cocos2d::CCPoint p0) {
            if (s_simulation.isSimulating()) return;
            
            PlayLayer::playEndAnimationToPos(p0);
        }

        void flipGravity(PlayerObject* p0, bool p1, bool p2) {
            if (s_simulation.isSimulating()) return;
            
            PlayLayer::flipGravity(p0, p1, p2);
        }

        void onQuit() {
            PlayLayer::onQuit();
            s_simulation.quit();
        }
    };
    
    class $modify(LevelEditorLayer) {
        bool init(GJGameLevel* level, bool unk) {
            bool result = LevelEditorLayer::init(level, unk);
            s_simulation.quit();
            return result;
        }
    };

    class $modify(HardStreak) {
        void addPoint(cocos2d::CCPoint p0) {
            if (s_simulation.isSimulating()) return;

            HardStreak::addPoint(p0);
        }
    };

    class $modify(EffectGameObject) {
        void triggerObject(GJBaseGameLayer* p0, int p1, const gd::vector<int>* p2) {
            if (s_simulation.isSimulating()) return;
            
            return EffectGameObject::triggerObject(p0, p1, p2);
        }
    };

    class $modify(GameObject) {
        void playShineEffect() {
            if (s_simulation.isSimulating()) return;
            
            GameObject::playShineEffect();
        }
    };

    class $modify(PlayerObject) {

        void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to) {
            if (s_simulation.isSimulating()) return;

            PlayerObject::playSpiderDashEffect(from, to);
        }

        void incrementJumps() {
            if (s_simulation.isSimulating()) return;
            
            PlayerObject::incrementJumps();
        }

        void update(float dt){
            PlayerObject::update(dt);

            if (PlayLayer::get() && !s_simulation.isSimulating())
                s_simulation.setFrameDelta(dt);
        }

        void ringJump(RingObject* p0, bool p1) {
            if (s_simulation.isSimulating()) return;
            
            PlayerObject::ringJump(p0, p1);
        }
    };

    class $modify(GJBaseGameLayer) {
        bool canBeActivatedByPlayer(PlayerObject* p0, EffectGameObject* p1) {
            if (s_simulation.isSimulating())
                return false;
            
            return GJBaseGameLayer::canBeActivatedByPlayer(p0, p1);
        }

        void handleButton(bool down, int button, bool isPlayer1){
            if (button == 1)
                s_simulation.handleButton(down, isPlayer1);

            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }

        //don't trigger objects early
        void collisionCheckObjects(PlayerObject* p0, gd::vector<GameObject*>* vec, int p2, float p3) {
            if (s_simulation.isSimulating()) {
                gd::vector<GameObject*> extra = *vec;

                auto new_end = std::remove_if(vec->begin(), vec->end(), [&](GameObject* p1) {
                    bool result = p1->m_objectType != GameObjectType::Solid &&
                                p1->m_objectType != GameObjectType::Hazard &&
                                p1->m_objectType != GameObjectType::AnimatedHazard &&
                                p1->m_objectType != GameObjectType::Slope;
                    return result;
                });

                vec->erase(new_end, vec->end());
                p2 = vec->size();

                GJBaseGameLayer::collisionCheckObjects(p0, vec, p2, p3);

                *vec = extra;
                return;
            }

            GJBaseGameLayer::collisionCheckObjects(p0, vec, p2, p3);
        }

        void playerTouchedRing(PlayerObject* player, RingObject* ring) {
            if (s_simulation.isSimulating()) return;

            GJBaseGameLayer::playerTouchedRing(player, ring);
        }

        void updateCamera(float dt) {
            if (config::get<bool>("player.showtrajectory", false))
                s_simulation.simulate();
            else
                s_simulation.hide();

            GJBaseGameLayer::updateCamera(dt);
        }
    };


}
