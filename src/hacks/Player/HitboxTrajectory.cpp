#include <modules/config/config.hpp>
#include <modules/gui/color.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/utils/RingBuffer.hpp>
#include <utils.hpp>

#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {

    struct HitboxFrame {
        std::array<cocos2d::CCPoint, 4> corners;
        float time;
        PlayerMode mode;
        float speed;
    };

    class HitboxTrajectoryManager {
    public:
        RingBuffer<HitboxFrame> m_player1Frames{120};
        RingBuffer<HitboxFrame> m_player2Frames{120};
        cocos2d::CCDrawNode* m_drawNode = nullptr;
        bool m_playerDead = false;
        int m_frameCount = 0;
        float m_currentTime = 0.f;

        static HitboxTrajectoryManager& get() {
            static HitboxTrajectoryManager instance;
            return instance;
        }

        void updateSizes() {
            size_t newSize = static_cast<size_t>(config::get<int>("player.hitboxtrajectory.length", 120));
            if (m_player1Frames.capacity() != newSize) {
                m_player1Frames.resize(newSize);
                m_player2Frames.resize(newSize);
            }
        }

        void reset() {
            m_player1Frames.clear();
            m_player2Frames.clear();
            m_playerDead = false;
            m_frameCount = 0;
            m_currentTime = 0.f;
            if (m_drawNode) {
                m_drawNode->clear();
            }
        }

        cocos2d::CCDrawNode* getDrawNode() {
            if (!m_drawNode) {
                m_drawNode = cocos2d::CCDrawNode::create();
                m_drawNode->retain();
                m_drawNode->setID("hitbox-trajectory-node"_spr);
                m_drawNode->setZOrder(100); // Draw above standard things
            }
            return m_drawNode;
        }

        void captureFrame(PlayerObject* player, RingBuffer<HitboxFrame>& buffer) {
            if (!player) return;

            int skipRate = config::get<int>("player.hitboxtrajectory.samplerate", 1);
            if (m_frameCount % skipRate != 0) return;

            HitboxFrame frame;
            frame.time = m_currentTime;
            frame.speed = player->m_isPlatformer ? std::abs(player->m_platformerXVelocity) : player->m_playerSpeed;
            frame.mode = utils::getGameMode(player);

            if (auto ob = player->m_orientedBox) {
                frame.corners = {ob->m_corners[0], ob->m_corners[1], ob->m_corners[2], ob->m_corners[3]};
            } else {
                auto rect = player->getObjectRect();
                frame.corners = {
                    cocos2d::CCPoint(rect.getMinX(), rect.getMinY()),
                    cocos2d::CCPoint(rect.getMinX(), rect.getMaxY()),
                    cocos2d::CCPoint(rect.getMaxX(), rect.getMaxY()),
                    cocos2d::CCPoint(rect.getMaxX(), rect.getMinY())
                };
            }

            buffer.push_back(frame);
        }

        void renderTrail(RingBuffer<HitboxFrame>& buffer) {
            if (buffer.empty()) return;

            float fadeSpeed = config::get<float>("player.hitboxtrajectory.fadespeed", 1.0f);
            gui::Color baseColor = config::get<gui::Color>("player.hitboxtrajectory.color", gui::Color(1, 0, 0, 1));
            float thickness = config::get<float>("player.hitboxtrajectory.thickness", 1.0f);
            bool velocityColor = config::get<bool>("player.hitboxtrajectory.velocitycolor", false);
            bool modeAware = config::get<bool>("player.hitboxtrajectory.modeaware", false);
            bool drawOutlineOnly = config::get<bool>("player.hitboxtrajectory.outlineonly", true);

            for (auto& frame : buffer) {
                float age = m_currentTime - frame.time;
                if (age < 0) age = 0;
                
                float alpha = 1.0f - (age * fadeSpeed);
                if (alpha <= 0.0f) continue;
                if (alpha > 1.0f) alpha = 1.0f;

                gui::Color color = baseColor;

                if (velocityColor) {
                    if (frame.speed < 0.8f) color = gui::Color(0, 0, 1, 1);
                    else if (frame.speed < 1.0f) color = gui::Color(0, 1, 1, 1);
                    else if (frame.speed < 1.2f) color = gui::Color(0, 1, 0, 1);
                    else if (frame.speed < 1.5f) color = gui::Color(1, 1, 0, 1);
                    else color = gui::Color(1, 0, 0, 1);
                } else if (modeAware) {
                    switch (frame.mode) {
                        case PlayerMode::Cube: color = gui::Color(0, 1, 0, 1); break;
                        case PlayerMode::Ship: color = gui::Color(1, 0, 1, 1); break;
                        case PlayerMode::UFO: color = gui::Color(1, 1, 0, 1); break;
                        case PlayerMode::Ball: color = gui::Color(1, 0.5f, 0, 1); break;
                        case PlayerMode::Wave: color = gui::Color(0, 1, 1, 1); break;
                        case PlayerMode::Robot: color = gui::Color(0.5f, 0, 1, 1); break;
                        case PlayerMode::Spider: color = gui::Color(1, 1, 1, 1); break;
                        case PlayerMode::Swing: color = gui::Color(0.5f, 0.5f, 0.5f, 1); break;
                    }
                }

                cocos2d::ccColor4F fillColor = {color.r, color.g, color.b, drawOutlineOnly ? 0.0f : alpha * 0.3f};
                cocos2d::ccColor4F borderColor = {color.r, color.g, color.b, alpha};

                m_drawNode->drawPolygon(frame.corners.data(), 4, fillColor, thickness, borderColor);
            }
        }
    };

    class $hack(HitboxTrajectory) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            auto toggle = tab->addToggle("player.hitboxtrajectory")->setDescription()->handleKeybinds();

            config::setIfEmpty("player.hitboxtrajectory.length", 120);
            config::setIfEmpty("player.hitboxtrajectory.fadespeed", 1.0f);
            config::setIfEmpty("player.hitboxtrajectory.color", gui::Color(1, 0, 0, 1));
            config::setIfEmpty("player.hitboxtrajectory.thickness", 1.0f);
            config::setIfEmpty("player.hitboxtrajectory.samplerate", 1);
            config::setIfEmpty("player.hitboxtrajectory.ondeath", false);
            config::setIfEmpty("player.hitboxtrajectory.practiceonly", false);
            config::setIfEmpty("player.hitboxtrajectory.outlineonly", true);
            config::setIfEmpty("player.hitboxtrajectory.velocitycolor", false);
            config::setIfEmpty("player.hitboxtrajectory.modeaware", false);

            toggle->addOptions([](auto options) {
                options->addInputInt("player.hitboxtrajectory.length", 10, 1000);
                options->addInputFloat("player.hitboxtrajectory.fadespeed", 0.1f, 10.0f, "%.1f");
                options->addInputInt("player.hitboxtrajectory.samplerate", 1, 10);
                options->addInputFloat("player.hitboxtrajectory.thickness", 0.5f, 5.0f, "%.1f");
                options->addColorComponent("player.hitboxtrajectory.color");
                
                options->addToggle("player.hitboxtrajectory.outlineonly");
                options->addToggle("player.hitboxtrajectory.ondeath");
                options->addToggle("player.hitboxtrajectory.practiceonly");
                options->addToggle("player.hitboxtrajectory.velocitycolor");
                options->addToggle("player.hitboxtrajectory.modeaware");
            });

            config::addDelegate("player.hitboxtrajectory", [] {
                auto value = config::get<bool>("player.hitboxtrajectory", false);
                HitboxTrajectoryManager::get().getDrawNode()->setVisible(value);
            });
        }

        [[nodiscard]] bool isCheating() const override { return config::get<bool>("player.hitboxtrajectory", false); }
        [[nodiscard]] const char* getId() const override { return "Show Hitbox Trajectory"; }
    };

    REGISTER_HACK(HitboxTrajectory)

    class $modify(HitboxTrajectoryPLHook, PlayLayer) {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
            HitboxTrajectoryManager::get().reset();
            return true;
        }

        void createObjectsFromSetupFinished() {
            PlayLayer::createObjectsFromSetupFinished();
            
            auto& manager = HitboxTrajectoryManager::get();
            auto node = manager.getDrawNode();
            if (node->getParent()) node->removeFromParent();
            
            if (m_objectLayer) {
                m_objectLayer->addChild(node, 100);
            }
        }

        void updateVisibility(float dt) override {
            PlayLayer::updateVisibility(dt);

            if (!config::get<bool>("player.hitboxtrajectory", false)) return;

            auto& manager = HitboxTrajectoryManager::get();

            if (config::get<bool>("player.hitboxtrajectory.practiceonly", false) && !m_isPracticeMode) {
                manager.getDrawNode()->clear();
                return;
            }

            manager.updateSizes();

            if (m_player1->m_isDead) {
                manager.m_playerDead = true;
            }

            if (!manager.m_playerDead) {
                manager.m_currentTime += dt;
            }

            manager.getDrawNode()->clear();

            bool showOnlyOnDeath = config::get<bool>("player.hitboxtrajectory.ondeath", false);
            bool shouldRender = !showOnlyOnDeath || manager.m_playerDead;

            if (shouldRender) {
                manager.renderTrail(manager.m_player1Frames);
                if (m_gameState.m_isDualMode) {
                    manager.renderTrail(manager.m_player2Frames);
                }
            }

            if (!manager.m_playerDead) {
                manager.captureFrame(m_player1, manager.m_player1Frames);
                if (m_gameState.m_isDualMode) {
                    manager.captureFrame(m_player2, manager.m_player2Frames);
                }
                manager.m_frameCount++;
            }
        }

        void resetLevel() override {
            PlayLayer::resetLevel();
            HitboxTrajectoryManager::get().reset();
        }

        void onQuit() {
            PlayLayer::onQuit();
            HitboxTrajectoryManager::get().reset();
        }
    };
}
