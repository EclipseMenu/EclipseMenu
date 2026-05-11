#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/utils/GameCheckpoint.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/UILayer.hpp>

using namespace geode::prelude;

namespace eclipse::Hacks::Level {
    static std::vector<GameObject*>& brokenPracticeObjects() {
        static std::vector<GameObject*> objects;
        return objects;
    }

    class $hack(PracticeFix) {
        static bool shouldEnable() {
            bool isRightMode = config::get<int>("bot.practice-fix-mode", 0) == 0;
            bool isBotRecording = config::get<int>("bot.state", 0) == 1;
            bool hasFix = config::get<bool>("bot.practicefix", false);

            if (isBotRecording) return isRightMode;
            return hasFix;
        }

        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("bot.practicefix")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Practice Fix"; }
    };

    REGISTER_HACK(PracticeFix)

    class CheckpointData {
    public:
        CheckpointData() = default;

        CheckpointData(
            PlayerObject* player1,
            PlayerObject* player2,
            PlayLayer* playLayer,
            std::vector<GameObject*> const& broken
        ) {
            m_checkpointPlayLayer = utils::FixPlayLayerCheckpoint(playLayer);
            m_checkpointPlayer1 = utils::FixPlayerCheckpoint(player1);

            if (player2)
                m_checkpointPlayer2 = utils::FixPlayerCheckpoint(player2);

            m_brokenObjects = broken;
        }

        void apply(PlayerObject* player1, PlayerObject* player2, PlayLayer* playLayer) {
            m_checkpointPlayLayer.apply(playLayer);
            m_checkpointPlayer1.apply(player1);

            if (player2)
                m_checkpointPlayer2.apply(player2);

            for (auto* obj : m_brokenObjects) {
                if (!obj)
                    continue;

                obj->m_isDisabled = true;
                obj->m_isDisabled2 = true;
                obj->setOpacity(0.f);
            }
        }

    private:
        utils::FixPlayerCheckpoint m_checkpointPlayer1;
        utils::FixPlayerCheckpoint m_checkpointPlayer2;
        utils::FixPlayLayerCheckpoint m_checkpointPlayLayer;

        std::vector<GameObject*> m_brokenObjects;
    };

    class $modify(FixPlayLayer, PlayLayer) {
        struct Fields {
            std::unordered_map<CheckpointObject*, CheckpointData> m_checkpoints;
        };

        void resetLevel() {
            if (!PracticeFix::shouldEnable()) {
                m_fields->m_checkpoints.clear();
                brokenPracticeObjects().clear();

                PlayLayer::resetLevel();
                return;
            }

            bool hadCheckpoints = m_checkpointArray->count() > 0;

            bool p1Holding = false;
            bool p2Holding = false;

            bool p1Left = false;
            bool p1Right = false;

            bool p2Left = false;
            bool p2Right = false;

            if (m_uiLayer) {
                p1Holding = m_uiLayer->m_p1Jumping || m_uiLayer->m_p1TouchId != -1;
                p2Holding = m_uiLayer->m_p2Jumping || m_uiLayer->m_p2TouchId != -1;

                if (m_player1) {
                    p1Left = m_player1->m_holdingLeft || m_player1->m_holdingButtons[2];
                    p1Right = m_player1->m_holdingRight || m_player1->m_holdingButtons[3];
                }

                if (m_player2) {
                    p2Left = m_player2->m_holdingLeft || m_player2->m_holdingButtons[2];
                    p2Right = m_player2->m_holdingRight || m_player2->m_holdingButtons[3];
                }
            }

            if (!hadCheckpoints) {
                m_fields->m_checkpoints.clear();
                brokenPracticeObjects().clear();
            }

            PlayLayer::resetLevel();

            if (hadCheckpoints) {
                m_resumeTimer = 0;

                if (!m_queuedButtons.empty())
                    m_queuedButtons.pop_back();
            }

            auto queueIfChanged = [this](int button, bool down, bool player2) {
                auto* player = player2 ? m_player2 : m_player1;

                if (!player)
                    return;

                if (player->m_holdingButtons[button] == down)
                    return;

                this->queueButton(button, down, player2, 0.0);
            };

            queueIfChanged(1, p1Holding, false);

            if (m_isPlatformer) {
                queueIfChanged(2, p1Left, false);
                queueIfChanged(3, p1Right, false);
            }

            if (m_gameState.m_isDualMode && m_levelSettings->m_twoPlayerMode) {
                queueIfChanged(1, p2Holding, true);

                if (m_isPlatformer) {
                    queueIfChanged(2, p2Left, true);
                    queueIfChanged(3, p2Right, true);
                }
            }
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            if (PracticeFix::shouldEnable()) {
                auto fields = m_fields.self();

                if (fields->m_checkpoints.contains(checkpoint)) {
                    PlayLayer::loadFromCheckpoint(checkpoint);

                    CheckpointData& data = fields->m_checkpoints[checkpoint];

                    data.apply(
                        m_player1,
                        m_gameState.m_isDualMode ? m_player2 : nullptr,
                        this
                    );

                    return;
                }
            }

            PlayLayer::loadFromCheckpoint(checkpoint);
        }

        CheckpointObject* createCheckpoint() {
            auto checkpoint = PlayLayer::createCheckpoint();

            if (!checkpoint || !PracticeFix::shouldEnable())
                return checkpoint;

            if (m_gameState.m_currentProgress > 0) {
                CheckpointData data(
                    m_player1,
                    m_gameState.m_isDualMode ? m_player2 : nullptr,
                    this,
                    brokenPracticeObjects()
                );

                m_fields->m_checkpoints[checkpoint] = std::move(data);
            }

            return checkpoint;
        }

        void removeCheckpoint(bool first) {
            CheckpointObject* checkpoint = nullptr;

            if (m_checkpointArray->count()) {
                if (first)
                    checkpoint = static_cast<CheckpointObject*>(
                        m_checkpointArray->objectAtIndex(0)
                    );
                else
                    checkpoint = static_cast<CheckpointObject*>(
                        m_checkpointArray->lastObject()
                    );
            }

            auto fields = m_fields.self();

            if (checkpoint && fields->m_checkpoints.contains(checkpoint)) {
                fields->m_checkpoints.erase(checkpoint);
            }

            PlayLayer::removeCheckpoint(first);
        }

    #ifdef GEODE_IS_DESKTOP
        void storeCheckpoint(CheckpointObject* checkpoint) {
            auto oldCheckpoint = static_cast<CheckpointObject*>(
                m_checkpointArray->objectAtIndex(0)
            );

            PlayLayer::storeCheckpoint(checkpoint);

            auto fields = m_fields.self();

            if (
                !m_checkpointArray->containsObject(oldCheckpoint) &&
                fields->m_checkpoints.contains(oldCheckpoint)
            ) {
                fields->m_checkpoints.erase(oldCheckpoint);
            }
        }
    #endif
    };

    class $modify(FixGJBaseGameLayer, GJBaseGameLayer) {
        void destroyObject(GameObject* obj) {
            if (PracticeFix::shouldEnable() && m_isPracticeMode) {
                brokenPracticeObjects().push_back(obj);
            }

            GJBaseGameLayer::destroyObject(obj);
        }
    };

    class $modify(FixPlayerObject, PlayerObject) {
        void releaseAllButtons() {
            if (!PracticeFix::shouldEnable()) {
                PlayerObject::releaseAllButtons();
                return;
            }

            auto* pl = utils::get<PlayLayer>();

            if (!pl) {
                PlayerObject::releaseAllButtons();
                return;
            }

            auto* gjbgl = utils::get<GJBaseGameLayer>();

            if (
                this == gjbgl->m_player2 &&
                !gjbgl->m_gameState.m_isDualMode
            ) {
                PlayerObject::releaseAllButtons();
                return;
            }

            bool isP2 = this == gjbgl->m_player2;

            bool holding = isP2
                ? (
                    gjbgl->m_uiLayer->m_p2Jumping ||
                    gjbgl->m_uiLayer->m_p2TouchId != -1
                )
                : (
                    gjbgl->m_uiLayer->m_p1Jumping ||
                    gjbgl->m_uiLayer->m_p1TouchId != -1
                );

            if (!holding)
                PlayerObject::releaseAllButtons();
        }
    };

    class $modify(FixUILayer, UILayer) {
        void handleKeypress(enumKeyCodes key, bool down, double timestamp) {
            auto* pl = utils::get<PlayLayer>();

            bool shouldPreserve =
                pl &&
                PracticeFix::shouldEnable() &&
                down &&
                key == enumKeyCodes::KEY_R &&
                pl->m_isPracticeMode &&
                pl->m_checkpointArray->count() > 0 &&
                (
                    m_p1Jumping ||
                    m_p2Jumping ||
                    m_p1TouchId != -1 ||
                    m_p2TouchId != -1
                );

            bool p1Jumping = m_p1Jumping;
            bool p2Jumping = m_p2Jumping;

            int p1TouchId = m_p1TouchId;
            int p2TouchId = m_p2TouchId;

            UILayer::handleKeypress(key, down, timestamp);

            if (!shouldPreserve)
                return;

            m_p1Jumping = p1Jumping;
            m_p2Jumping = p2Jumping;

            m_p1TouchId = p1TouchId;
            m_p2TouchId = p2TouchId;
        }
    };
}
