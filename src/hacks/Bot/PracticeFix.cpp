#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CheckpointObject.hpp>

using namespace geode::prelude;

namespace eclipse::Hacks::Bot {

    class PracticeFix : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bot");
            tab->addToggle("Practice Fix", "bot.practicefix")->setDescription("Properly saves the player\'s velocity when respawning from a checkpoint.");
        }

        void update() override {}
        [[nodiscard]] bool isCheating() override { return false; }
        [[nodiscard]] const char* getId() const override { return "Practice Fix"; }
    };

    REGISTER_HACK(PracticeFix)

    class FixPlayerCheckpoint {
    public:
        FixPlayerCheckpoint() = default;

        FixPlayerCheckpoint(PlayerObject* player) {
            m_unk518 = player->m_unk518;
            m_unk51c = player->m_unk51c;
            m_unk51d = player->m_unk51d;
            m_unk51e = player->m_unk51e;
            m_unk51f = player->m_unk51f;
            m_unk568 = player->m_unk568;
            unk_584 = player->unk_584;
            unk_588 = player->unk_588;
            m_rotationSpeed = player->m_rotationSpeed;
            m_unk5dc = player->m_unk5dc;
            m_isRotating = player->m_isRotating;
            m_unk5e1 = player->m_unk5e1;
            m_hasGlow = player->m_hasGlow;
            m_isHidden = player->m_isHidden;
            m_hasGhostTrail = player->m_hasGhostTrail;
            m_unk62c = player->m_unk62c;
            m_unk630 = player->m_unk630;
            m_unk634 = player->m_unk634;
            m_unk638 = player->m_unk638;
            m_unk63c = player->m_unk63c;
            m_unk640 = player->m_unk640;
            m_unk644 = player->m_unk644;
            m_unk648 = player->m_unk648;
            m_unk658 = player->m_unk658;
            m_unk659 = player->m_unk659;
            m_unk65a = player->m_unk65a;
            m_unk65b = player->m_unk65b;
            m_playEffects = player->m_playEffects;
            m_unk65d = player->m_unk65d;
            m_unk65e = player->m_unk65e;
            m_unk65f = player->m_unk65f;
            m_unk688 = player->m_unk688;
            m_unk68c = player->m_unk68c;
            m_gv0096 = player->m_gv0096;
            m_gv0100 = player->m_gv0100;
            m_unk6c0 = player->m_unk6c0;
            m_unk70c = player->m_unk70c;
            m_unk710 = player->m_unk710;
            m_playerStreak = player->m_playerStreak;
            m_hasCustomGlowColor = player->m_hasCustomGlowColor;
            m_isShip = player->m_isShip;
            m_isBird = player->m_isBird;
            m_isBall = player->m_isBall;
            m_isDart = player->m_isDart;
            m_isRobot = player->m_isRobot;
            m_isSpider = player->m_isSpider;
            m_isUpsideDown = player->m_isUpsideDown;
            m_isDead = player->m_isDead;
            m_isOnGround = player->m_isOnGround;
            m_isGoingLeft = player->m_isGoingLeft;
            m_isSideways = player->m_isSideways;
            m_isSwing = player->m_isSwing;
            m_unk7c8 = player->m_unk7c8;
            m_unk7cc = player->m_unk7cc;
            m_unk7d0 = player->m_unk7d0;
            m_isDashing = player->m_isDashing;
            m_vehicleSize = player->m_vehicleSize;
            m_playerSpeed = player->m_playerSpeed;
            m_unk7e0 = player->m_unk7e0;
            m_unk7e4 = player->m_unk7e4;
            m_unk7e8 = player->m_unk7e8;
            m_unk7f0 = player->m_unk7f0;
            m_unk7f8 = player->m_unk7f8;
            m_isLocked = player->m_isLocked;
            m_unka2b = player->m_unka2b;
            m_lastGroundedPos = player->m_lastGroundedPos;
            m_unk814 = player->m_unk814;
            m_unk815 = player->m_unk815;
            m_gamevar0060 = player->m_gamevar0060;
            m_gamevar0061 = player->m_gamevar0061;
            m_gamevar0062 = player->m_gamevar0062;
            m_unk838 = player->m_unk838;
            m_isPlatformer = player->m_isPlatformer;
            m_unk8ec = player->m_unk8ec;
            m_unk8f0 = player->m_unk8f0;
            m_unk8f4 = player->m_unk8f4;
            m_unk8f8 = player->m_unk8f8;
            m_gravityMod = player->m_gravityMod;
            m_unk904 = player->m_unk904;
            m_unk918 = player->m_unk918;
            m_unk91c = player->m_unk91c;
            m_unk948 = player->m_unk948;
            m_iconRequestID = player->m_iconRequestID;
            m_unk974 = player->m_unk974;
            m_unk978 = player->m_unk978;
            m_unk979 = player->m_unk979;
            m_unk97a = player->m_unk97a;
            m_unk97b = player->m_unk97b;

#ifndef GEODE_IS_ANDROID
            //m_unk6a4 = player->m_unk6a4;
            //m_unk828 = player->m_unk828;
#else
            // gd::set, gd::unordered_set and gd::unordered_map are just type aliases of arrays of void* on android
            // until that is fixed, this is a workaround

            std::copy(std::begin(player->m_unk6a4), std::end(player->m_unk6a4), std::begin(m_unk6a4));
            std::copy(std::begin(player->m_unk828), std::end(player->m_unk828), std::begin(m_unk828));
#endif
            //m_unk880 = player->m_unk880;
            //m_unk910 = player->m_unk910;
            //m_unk924 = player->m_unk924;

            m_xVelocity = player->m_platformerXVelocity;
            m_yVelocity = player->m_yVelocity;
            m_xPosition = player->m_position.x;
            m_yPosition = player->m_position.y;
            m_rotationSpeed = player->m_rotationSpeed;
            m_rotation = player->getRotation();
            m_wasOnSlope = player->m_wasOnSlope;
            m_isOnSlope = player->m_isOnSlope;
            m_lastSnappedTo = player->m_objectSnappedTo;
        }

        void apply(PlayerObject* player) {
            player->m_unk518 = m_unk518;
            player->m_unk51c = m_unk51c;
            player->m_unk51d = m_unk51d;
            player->m_unk51e = m_unk51e;
            player->m_unk51f = m_unk51f;
            player->m_unk568 = m_unk568;
            player->unk_584 = unk_584;
            player->unk_588 = unk_588;
            player->m_rotationSpeed = m_rotationSpeed;
            player->m_unk5dc = m_unk5dc;
            player->m_isRotating = m_isRotating;
            player->m_unk5e1 = m_unk5e1;
            player->m_hasGlow = m_hasGlow;
            player->m_isHidden = m_isHidden;
            player->m_hasGhostTrail = m_hasGhostTrail;
            player->m_unk62c = m_unk62c;
            player->m_unk630 = m_unk630;
            player->m_unk634 = m_unk634;
            player->m_unk638 = m_unk638;
            player->m_unk63c = m_unk63c;
            player->m_unk640 = m_unk640;
            player->m_unk644 = m_unk644;
            player->m_unk648 = m_unk648;
            player->m_unk658 = m_unk658;
            player->m_unk659 = m_unk659;
            player->m_unk65a = m_unk65a;
            player->m_unk65b = m_unk65b;
            player->m_playEffects = m_playEffects;
            player->m_unk65d = m_unk65d;
            player->m_unk65e = m_unk65e;
            player->m_unk65f = m_unk65f;
            player->m_unk688 = m_unk688;
            player->m_unk68c = m_unk68c;
            player->m_gv0096 = m_gv0096;
            player->m_gv0100 = m_gv0100;
            player->m_unk6c0 = m_unk6c0;
            player->m_unk70c = m_unk70c;
            player->m_unk710 = m_unk710;
            player->m_playerStreak = m_playerStreak;
            player->m_hasCustomGlowColor = m_hasCustomGlowColor;
            player->m_isShip = m_isShip;
            player->m_isBird = m_isBird;
            player->m_isBall = m_isBall;
            player->m_isDart = m_isDart;
            player->m_isRobot = m_isRobot;
            player->m_isSpider = m_isSpider;
            player->m_isUpsideDown = m_isUpsideDown;
            player->m_isDead = m_isDead;
            player->m_isOnGround = m_isOnGround;
            player->m_isGoingLeft = m_isGoingLeft;
            player->m_isSideways = m_isSideways;
            player->m_isSwing = m_isSwing;
            player->m_unk7c8 = m_unk7c8;
            player->m_unk7cc = m_unk7cc;
            player->m_unk7d0 = m_unk7d0;
            player->m_isDashing = m_isDashing;
            player->m_vehicleSize = m_vehicleSize;
            player->m_playerSpeed = m_playerSpeed;
            player->m_unk7e0 = m_unk7e0;
            player->m_unk7e4 = m_unk7e4;
            player->m_unk7e8 = m_unk7e8;
            player->m_unk7f0 = m_unk7f0;
            player->m_unk7f8 = m_unk7f8;
            player->m_isLocked = m_isLocked;
            player->m_unka2b = m_unka2b;
            player->m_lastGroundedPos = m_lastGroundedPos;
            player->m_unk814 = m_unk814;
            player->m_unk815 = m_unk815;
            player->m_gamevar0060 = m_gamevar0060;
            player->m_gamevar0061 = m_gamevar0061;
            player->m_gamevar0062 = m_gamevar0062;
            player->m_unk838 = m_unk838;
            player->m_isPlatformer = m_isPlatformer;
            player->m_unk8ec = m_unk8ec;
            player->m_unk8f0 = m_unk8f0;
            player->m_unk8f4 = m_unk8f4;
            player->m_unk8f8 = m_unk8f8;
            player->m_gravityMod = m_gravityMod;
            player->m_unk904 = m_unk904;
            player->m_unk918 = m_unk918;
            player->m_unk91c = m_unk91c;
            player->m_unk948 = m_unk948;
            player->m_iconRequestID = m_iconRequestID;
            player->m_unk974 = m_unk974;
            player->m_unk978 = m_unk978;
            player->m_unk979 = m_unk979;
            player->m_unk97a = m_unk97a;
            player->m_unk97b = m_unk97b;

#ifndef GEODE_IS_ANDROID
            //player->m_unk6a4 = m_unk6a4;
            //player->m_unk828 = m_unk828;
#else
            std::copy(std::begin(m_unk6a4), std::end(m_unk6a4), std::begin(player->m_unk6a4));
            std::copy(std::begin(m_unk828), std::end(m_unk828), std::begin(player->m_unk828));
#endif
            //player->m_unk880 = m_unk880;
            //player->m_unk910 = m_unk910;
            //player->m_unk924 = m_unk924;

            player->m_platformerXVelocity = m_xVelocity;
            player->m_yVelocity = m_yVelocity;
            player->setPositionX(m_xPosition);
            player->setPositionY(m_yPosition);
            player->m_position.x = m_xPosition;
            player->m_position.y = m_yPosition;
            player->setRotation(m_rotation);
            player->m_rotationSpeed = m_rotationSpeed;
            player->m_wasOnSlope = m_wasOnSlope;
            player->m_isOnSlope = m_isOnSlope;
            player->m_objectSnappedTo = m_lastSnappedTo;
        }

    private:
        float m_unk518;
        bool m_unk51c;
        bool m_unk51d;
        bool m_unk51e;
        bool m_unk51f;
        float m_unk568;
        float unk_584;
        int unk_588;
        float m_rotationSpeed;
        float m_unk5dc;
        bool m_isRotating;
        bool m_unk5e1;
        bool m_hasGlow;
        bool m_isHidden;
        int m_hasGhostTrail;
        float m_unk62c;
        int m_unk630;
        float m_unk634;
        int m_unk638;
        float m_unk63c;
        int m_unk640;
        float m_unk644;
        float m_unk648;
        bool m_unk658;
        bool m_unk659;
        bool m_unk65a;
        bool m_unk65b;
        bool m_playEffects;
        bool m_unk65d;
        bool m_unk65e;
        bool m_unk65f;
        float m_unk688;
        float m_unk68c;
        bool m_gv0096;
        bool m_gv0100;
        int m_unk6c0;
        float m_unk70c;
        float m_unk710;
        int m_playerStreak;
        bool m_hasCustomGlowColor;
        bool m_isShip;
        bool m_isBird;
        bool m_isBall;
        bool m_isDart;
        bool m_isRobot;
        bool m_isSpider;
        bool m_isUpsideDown;
        bool m_isDead;
        bool m_isOnGround;
        bool m_isGoingLeft;
        bool m_isSideways;
        bool m_isSwing;
        float m_unk7c8;
        float m_unk7cc;
        float m_unk7d0;
        bool m_isDashing;
        float m_vehicleSize;
        float m_playerSpeed;
        float m_unk7e0;
        float m_unk7e4;
        cocos2d::CCPoint m_unk7e8;
        cocos2d::CCPoint m_unk7f0;
        bool m_unk7f8;
        bool m_isLocked;
        bool m_unka2b;
        cocos2d::CCPoint m_lastGroundedPos;
        bool m_unk814;
        bool m_unk815;
        bool m_gamevar0060;
        bool m_gamevar0061;
        bool m_gamevar0062;
        float m_unk838;
        bool m_isPlatformer;
        int m_unk8ec;
        int m_unk8f0;
        int m_unk8f4;
        int m_unk8f8;
        float m_gravityMod;
        cocos2d::CCPoint m_unk904;
        float m_unk918;
        float m_unk91c;
        bool m_unk948;
        int m_iconRequestID;
        float m_unk974;
        bool m_unk978;
        bool m_unk979;
        bool m_unk97a;
        bool m_unk97b;

        gd::unordered_set<int> m_unk6a4;
        gd::unordered_set<int> m_unk828;
        gd::vector<float> m_unk880;
        gd::map<int, bool> m_unk910;
        gd::map<int, bool> m_unk924;

        double m_xVelocity;
        double m_yVelocity;
        float m_xPosition;
        float m_yPosition;
        float m_rotation;

        GameObject* m_lastSnappedTo = nullptr;
        GameObject* m_lastSnappedTo2 = nullptr;

        bool m_isOnSlope;
        bool m_wasOnSlope;
    };

    class CheckpointData {
    public:
        CheckpointData() = default;

        CheckpointData(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1 = FixPlayerCheckpoint(player1);
            m_checkpointPlayer2 = FixPlayerCheckpoint(player2);
        }

        void apply(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1.apply(player1);
            m_checkpointPlayer2.apply(player2);
        }

    private:
        FixPlayerCheckpoint m_checkpointPlayer1;
        FixPlayerCheckpoint m_checkpointPlayer2;
    };

    class $modify(FixPlayLayer, PlayLayer) {
        struct Fields {
            std::unordered_map<CheckpointObject*, CheckpointData> m_checkpoints;
        };

        void onQuit() {
            m_fields->m_checkpoints.clear();
            PlayLayer::onQuit();
        }

        void resetLevel() {
            if(m_checkpointArray->count() <= 0)
                m_fields->m_checkpoints.clear();

            PlayLayer::resetLevel();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            FixPlayLayer* playLayer = ((FixPlayLayer*)FixPlayLayer::get());

            if(config::get<bool>("bot.practicefix", false) && playLayer->m_fields->m_checkpoints.contains(checkpoint)) {
                PlayLayer::loadFromCheckpoint(checkpoint);

                CheckpointData data = playLayer->m_fields->m_checkpoints[checkpoint];
                data.apply(playLayer->m_player1, playLayer->m_player2);

                return;
            }

            PlayLayer::loadFromCheckpoint(checkpoint);
        }

    };

    class $modify(CheckpointObject) {
        bool init() {
            bool result = CheckpointObject::init();

            if (!config::get<bool>("bot.practicefix", false))
                return result;

            FixPlayLayer* playLayer = ((FixPlayLayer*)FixPlayLayer::get());

            if(playLayer->m_gameState.m_currentProgress > 0) {
                CheckpointData data(playLayer->m_player1, playLayer->m_player2);
                playLayer->m_fields->m_checkpoints[this] = data;
            }

            return result;
        }
    };

}
