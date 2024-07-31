#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/CheckpointObject.hpp>

using namespace geode::prelude;

namespace eclipse::Hacks::Level {

    class PracticeFix : public hack::Hack {
    public:
        static bool shouldEnable() {
            return config::get<bool>("bot.practicefix", false) || config::get<int>("bot.state", 0) == 1;
        }
        
    private:
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Practice Fix", "bot.practicefix")
                ->setDescription("Properly saves and restores the player's data when respawning from a checkpoint.");
        }

        [[nodiscard]] bool isCheating() override { return false; }
        [[nodiscard]] const char* getId() const override { return "Practice Fix"; }
    };

    REGISTER_HACK(PracticeFix)

    class FixPlayerCheckpoint {
    public:
        FixPlayerCheckpoint() = default;

        FixPlayerCheckpoint(PlayerObject* player) {
            m_wasTeleported = player->m_wasTeleported;
            m_fixGravityBug = player->m_fixGravityBug;
            m_reverseSync = player->m_reverseSync;
            m_yVelocityBeforeSlope = player->m_yVelocityBeforeSlope;
            m_dashX = player->m_dashX;
            m_dashY = player->m_dashY;
            m_dashAngle = player->m_dashAngle;
            m_dashStartTime = player->m_dashStartTime;
            m_slopeStartTime = player->m_slopeStartTime;
            m_justPlacedStreak = player->m_justPlacedStreak;
            m_maybeLastGroundObject = player->m_maybeLastGroundObject;
            m_lastCollisionBottom = player->m_lastCollisionBottom;
            m_lastCollisionTop = player->m_lastCollisionTop;
            m_lastCollisionLeft = player->m_lastCollisionLeft;
            m_lastCollisionRight = player->m_lastCollisionRight;
            m_unk50C = player->m_unk50C;
            m_unk510 = player->m_unk510;
            m_currentSlope2 = player->m_currentSlope2;
            m_preLastGroundObject = player->m_preLastGroundObject;
            m_slopeAngle = player->m_slopeAngle;
            m_slopeSlidingMaybeRotated = player->m_slopeSlidingMaybeRotated;
            m_quickCheckpointMode = player->m_quickCheckpointMode;
            m_collidedObject = player->m_collidedObject;
            m_lastGroundObject = player->m_lastGroundObject;
            m_collidingWithLeft = player->m_collidingWithLeft;
            m_collidingWithRight = player->m_collidingWithRight;
            m_maybeSavedPlayerFrame = player->m_maybeSavedPlayerFrame;
            m_scaleXRelated2 = player->m_scaleXRelated2;
            m_groundYVelocity = player->m_groundYVelocity;
            m_yVelocityRelated = player->m_yVelocityRelated;
            m_scaleXRelated3 = player->m_scaleXRelated3;
            m_scaleXRelated4 = player->m_scaleXRelated4;
            m_scaleXRelated5 = player->m_scaleXRelated5;
            m_isCollidingWithSlope = player->m_isCollidingWithSlope;
            m_isBallRotating = player->m_isBallRotating;
            m_unk669 = player->m_unk669;
            m_currentSlope3 = player->m_currentSlope3;
            m_currentSlope = player->m_currentSlope;
            unk_584 = player->unk_584;
            m_collidingWithSlopeId = player->m_collidingWithSlopeId;
            m_slopeFlipGravityRelated = player->m_slopeFlipGravityRelated;
            m_slopeAngleRadians = player->m_slopeAngleRadians;
            m_rotationSpeed = player->m_rotationSpeed;
            m_rotateSpeed = player->m_rotateSpeed;
            m_isRotating = player->m_isRotating;
            m_isBallRotating2 = player->m_isBallRotating2;
            m_hasGlow = player->m_hasGlow;
            m_isHidden = player->m_isHidden;
            m_ghostType = player->m_ghostType;
            m_speedMultiplier = player->m_speedMultiplier;
            m_yStart = player->m_yStart;
            m_gravity = player->m_gravity;
            m_trailingParticleLife = player->m_trailingParticleLife;
            m_unk648 = player->m_unk648;
            m_gameModeChangedTime = player->m_gameModeChangedTime;
            m_padRingRelated = player->m_padRingRelated;
            m_maybeReducedEffects = player->m_maybeReducedEffects;
            m_maybeIsFalling = player->m_maybeIsFalling;
            m_shouldTryPlacingCheckpoint = player->m_shouldTryPlacingCheckpoint;
            m_playEffects = player->m_playEffects;
            m_maybeCanRunIntoBlocks = player->m_maybeCanRunIntoBlocks;
            m_hasGroundParticles = player->m_hasGroundParticles;
            m_hasShipParticles = player->m_hasShipParticles;
            m_isOnGround3 = player->m_isOnGround3;
            m_checkpointTimeout = player->m_checkpointTimeout;
            m_lastCheckpointTime = player->m_lastCheckpointTime;
            m_lastJumpTime = player->m_lastJumpTime;
            m_lastFlipTime = player->m_lastFlipTime;
            m_flashTime = player->m_flashTime;
            m_flashRelated = player->m_flashRelated;
            m_flashRelated1 = player->m_flashRelated1;
            m_colorRelated2 = player->m_colorRelated2;
            m_flashRelated3 = player->m_flashRelated3;
            m_lastSpiderFlipTime = player->m_lastSpiderFlipTime;
            m_unkBool5 = player->m_unkBool5;
            m_maybeIsVehicleGlowing = player->m_maybeIsVehicleGlowing;
            m_gv0096 = player->m_gv0096;
            m_gv0100 = player->m_gv0100;
            m_accelerationOrSpeed = player->m_accelerationOrSpeed;
            m_snapDistance = player->m_snapDistance;
            m_ringJumpRelated = player->m_ringJumpRelated;
            m_objectSnappedTo = player->m_objectSnappedTo;
            m_pendingCheckpoint = player->m_pendingCheckpoint;
            m_onFlyCheckpointTries = player->m_onFlyCheckpointTries;
            m_maybeSpriteRelated = player->m_maybeSpriteRelated;
            m_useLandParticles0 = player->m_useLandParticles0;
            m_landParticlesAngle = player->m_landParticlesAngle;
            m_landParticleRelatedY = player->m_landParticleRelatedY;
            m_playerStreak = player->m_playerStreak;
            m_streakRelated1 = player->m_streakRelated1;
            m_streakRelated2 = player->m_streakRelated2;
            m_streakRelated3 = player->m_streakRelated3;
            m_shipStreakType = player->m_shipStreakType;
            m_slopeRotation = player->m_slopeRotation;
            m_currentSlopeYVelocity = player->m_currentSlopeYVelocity;
            m_unk3d0 = player->m_unk3d0;
            m_blackOrbRelated = player->m_blackOrbRelated;
            m_unk3e0 = player->m_unk3e0;
            m_unk3e1 = player->m_unk3e1;
            m_isAccelerating = player->m_isAccelerating;
            m_isCurrentSlopeTop = player->m_isCurrentSlopeTop;
            m_collidedTopMinY = player->m_collidedTopMinY;
            m_collidedBottomMaxY = player->m_collidedBottomMaxY;
            m_collidedLeftMaxX = player->m_collidedLeftMaxX;
            m_collidedRightMinX = player->m_collidedRightMinX;
            m_streakRelated4 = player->m_streakRelated4;
            // m_canPlaceCheckpoint = player->m_canPlaceCheckpoint;
            m_colorRelated = player->m_colorRelated;
            m_secondColorRelated = player->m_secondColorRelated;
            m_hasCustomGlowColor = player->m_hasCustomGlowColor;
            m_glowColor = player->m_glowColor;
            m_maybeIsColliding = player->m_maybeIsColliding;
            // m_jumpBuffered = player->m_jumpBuffered;
            m_stateRingJump = player->m_stateRingJump;
            m_wasJumpBuffered = player->m_wasJumpBuffered;
            m_wasRobotJump = player->m_wasRobotJump;
            m_stateJumpBuffered = player->m_stateJumpBuffered;
            m_stateRingJump2 = player->m_stateRingJump2;
            m_touchedRing = player->m_touchedRing;
            m_touchedCustomRing = player->m_touchedCustomRing;
            m_touchedGravityPortal = player->m_touchedGravityPortal;
            m_maybeTouchedBreakableBlock = player->m_maybeTouchedBreakableBlock;
            m_jumpRelatedAC2 = player->m_jumpRelatedAC2;
            m_touchedPad = player->m_touchedPad;
            m_yVelocity = player->m_yVelocity;
            m_fallSpeed = player->m_fallSpeed;
            m_isOnSlope = player->m_isOnSlope;
            m_wasOnSlope = player->m_wasOnSlope;
            m_slopeVelocity = player->m_slopeVelocity;
            m_maybeUpsideDownSlope = player->m_maybeUpsideDownSlope;
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
            m_reverseRelated = player->m_reverseRelated;
            m_maybeReverseSpeed = player->m_maybeReverseSpeed;
            m_maybeReverseAcceleration = player->m_maybeReverseAcceleration;
            m_xVelocityRelated2 = player->m_xVelocityRelated2;
            m_isDashing = player->m_isDashing;
            m_unk9e8 = player->m_unk9e8;
            m_groundObjectMaterial = player->m_groundObjectMaterial;
            m_vehicleSize = player->m_vehicleSize;
            m_playerSpeed = player->m_playerSpeed;
            m_shipRotation = player->m_shipRotation;
            m_lastPortalPos = player->m_lastPortalPos;
            m_unkUnused3 = player->m_unkUnused3;
            m_isOnGround2 = player->m_isOnGround2;
            m_lastLandTime = player->m_lastLandTime;
            m_platformerVelocityRelated = player->m_platformerVelocityRelated;
            m_maybeIsBoosted = player->m_maybeIsBoosted;
            m_scaleXRelatedTime = player->m_scaleXRelatedTime;
            m_decreaseBoostSlide = player->m_decreaseBoostSlide;
            m_unkA29 = player->m_unkA29;
            m_isLocked = player->m_isLocked;
            m_controlsDisabled = player->m_controlsDisabled;
            m_lastGroundedPos = player->m_lastGroundedPos;
            m_lastActivatedPortal = player->m_lastActivatedPortal;
            m_hasEverJumped = player->m_hasEverJumped;
            m_ringOrStreakRelated = player->m_ringOrStreakRelated;
            m_playerColor1 = player->m_playerColor1;
            m_playerColor2 = player->m_playerColor2;
            m_position = player->m_position;
            m_isSecondPlayer = player->m_isSecondPlayer;
            m_unkA99 = player->m_unkA99;
            m_totalTime = player->m_totalTime;
            m_isBeingSpawnedByDualPortal = player->m_isBeingSpawnedByDualPortal;
            m_unkAAC = player->m_unkAAC;
            m_unkAngle1 = player->m_unkAngle1;
            m_yVelocityRelated3 = player->m_yVelocityRelated3;
            m_gamevar0060 = player->m_gamevar0060;
            m_swapColors = player->m_swapColors;
            m_gamevar0062 = player->m_gamevar0062;
            m_followRelated = player->m_followRelated;
            m_playerFollowFloats = player->m_playerFollowFloats;
            m_unk838 = player->m_unk838;
            m_stateOnGround = player->m_stateOnGround;
            m_stateUnk = player->m_stateUnk;
            m_stateNoStickX = player->m_stateNoStickX;
            m_stateNoStickY = player->m_stateNoStickY;
            m_stateUnk2 = player->m_stateUnk2;
            m_stateBoostX = player->m_stateBoostX;
            m_stateBoostY = player->m_stateBoostY;
            m_maybeStateForce2 = player->m_maybeStateForce2;
            m_stateScale = player->m_stateScale;
            m_platformerXVelocity = player->m_platformerXVelocity;
            m_holdingRight = player->m_holdingRight;
            m_holdingLeft = player->m_holdingLeft;
            m_leftPressedFirst = player->m_leftPressedFirst;
            m_scaleXRelated = player->m_scaleXRelated;
            m_maybeHasStopped = player->m_maybeHasStopped;
            m_xVelocityRelated = player->m_xVelocityRelated;
            m_maybeGoingCorrectSlopeDirection = player->m_maybeGoingCorrectSlopeDirection;
            m_isSliding = player->m_isSliding;
            m_maybeSlopeForce = player->m_maybeSlopeForce;
            m_isOnIce = player->m_isOnIce;
            m_physDeltaRelated = player->m_physDeltaRelated;
            m_isOnGround4 = player->m_isOnGround4;
            m_maybeSlidingTime = player->m_maybeSlidingTime;
            m_maybeSlidingStartTime = player->m_maybeSlidingStartTime;
            m_changedDirectionsTime = player->m_changedDirectionsTime;
            m_slopeEndTime = player->m_slopeEndTime;
            m_isMoving = player->m_isMoving;
            m_platformerMovingLeft = player->m_platformerMovingLeft;
            m_platformerMovingRight = player->m_platformerMovingRight;
            m_isSlidingRight = player->m_isSlidingRight;
            m_maybeChangedDirectionAngle = player->m_maybeChangedDirectionAngle;
            m_unkUnused2 = player->m_unkUnused2;
            m_isPlatformer = player->m_isPlatformer;
            m_stateNoAutoJump = player->m_stateNoAutoJump;
            m_stateDartSlide = player->m_stateDartSlide;
            m_stateHitHead = player->m_stateHitHead;
            m_stateFlipGravity = player->m_stateFlipGravity;
            m_gravityMod = player->m_gravityMod;
            m_stateForce = player->m_stateForce;
            m_stateForceVector = player->m_stateForceVector;
            m_affectedByForces = player->m_affectedByForces;
            m_jumpPadRelated = player->m_jumpPadRelated; // map<int, bool>
            m_somethingPlayerSpeedTime = player->m_somethingPlayerSpeedTime;
            m_playerSpeedAC = player->m_playerSpeedAC;
            m_fixRobotJump = player->m_fixRobotJump;
            // m_holdingButtons = player->m_holdingButtons; // map<int, bool>
            m_inputsLocked = player->m_inputsLocked;
            m_currentRobotAnimation = player->m_currentRobotAnimation;
            m_gv0123 = player->m_gv0123;
            m_iconRequestID = player->m_iconRequestID;
            m_unkUnused = player->m_unkUnused;
            m_isOutOfBounds = player->m_isOutOfBounds;
            m_fallStartY = player->m_fallStartY;
            m_disablePlayerSqueeze = player->m_disablePlayerSqueeze;
            m_robotHasRun3 = player->m_robotHasRun3;
            m_robotHasRun2 = player->m_robotHasRun2;
            m_item20 = player->m_item20;
            m_ignoreDamage = player->m_ignoreDamage;
            m_enable22Changes = player->m_enable22Changes;

            m_xPosition = player->getPositionX();
            m_yPosition = player->getPositionY();
            m_rotation = player->getRotation();

#ifndef GEODE_IS_ANDROID
            m_rotateObjectsRelated = player->m_rotateObjectsRelated; // unordered_map<int, GJPointDouble>
            m_maybeRotatedObjectsMap = player->m_maybeRotatedObjectsMap; // unordered_map<int, GameObject*>
            m_ringRelatedSet = player->m_ringRelatedSet; // unordered_set<int>
            m_touchedRings = player->m_touchedRings; // unordered_set<int>
#else
            // gd::set, gd::unordered_set and gd::unordered_map are just type aliases of arrays of void* on android
            // until that is fixed, this is a workaround

            std::copy(std::begin(player->m_rotateObjectsRelated), std::end(player->m_rotateObjectsRelated), std::begin(m_rotateObjectsRelated));
            std::copy(std::begin(player->m_maybeRotatedObjectsMap), std::end(player->m_maybeRotatedObjectsMap), std::begin(m_maybeRotatedObjectsMap));
            std::copy(std::begin(player->m_ringRelatedSet), std::end(player->m_ringRelatedSet), std::begin(m_ringRelatedSet));
            std::copy(std::begin(player->m_touchedRings), std::end(player->m_touchedRings), std::begin(m_touchedRings));
#endif
        }

        void apply(PlayerObject* player) {
            player->m_wasTeleported = m_wasTeleported;
            player->m_fixGravityBug = m_fixGravityBug;
            player->m_reverseSync = m_reverseSync;
            player->m_yVelocityBeforeSlope = m_yVelocityBeforeSlope;
            player->m_dashX = m_dashX;
            player->m_dashY = m_dashY;
            player->m_dashAngle = m_dashAngle;
            player->m_dashStartTime = m_dashStartTime;
            player->m_slopeStartTime = m_slopeStartTime;
            player->m_justPlacedStreak = m_justPlacedStreak;
            player->m_maybeLastGroundObject = m_maybeLastGroundObject;
            player->m_lastCollisionBottom = m_lastCollisionBottom;
            player->m_lastCollisionTop = m_lastCollisionTop;
            player->m_lastCollisionLeft = m_lastCollisionLeft;
            player->m_lastCollisionRight = m_lastCollisionRight;
            player->m_unk50C = m_unk50C;
            player->m_unk510 = m_unk510;
            player->m_currentSlope2 = m_currentSlope2;
            player->m_preLastGroundObject = m_preLastGroundObject;
            player->m_slopeAngle = m_slopeAngle;
            player->m_slopeSlidingMaybeRotated = m_slopeSlidingMaybeRotated;
            player->m_quickCheckpointMode = m_quickCheckpointMode;
            player->m_collidedObject = m_collidedObject;
            player->m_lastGroundObject = m_lastGroundObject;
            player->m_collidingWithLeft = m_collidingWithLeft;
            player->m_collidingWithRight = m_collidingWithRight;
            player->m_maybeSavedPlayerFrame = m_maybeSavedPlayerFrame;
            player->m_scaleXRelated2 = m_scaleXRelated2;
            player->m_groundYVelocity = m_groundYVelocity;
            player->m_yVelocityRelated = m_yVelocityRelated;
            player->m_scaleXRelated3 = m_scaleXRelated3;
            player->m_scaleXRelated4 = m_scaleXRelated4;
            player->m_scaleXRelated5 = m_scaleXRelated5;
            player->m_isCollidingWithSlope = m_isCollidingWithSlope;
            player->m_isBallRotating = m_isBallRotating;
            player->m_unk669 = m_unk669;
            player->m_currentSlope3 = m_currentSlope3;
            player->m_currentSlope = m_currentSlope;
            player->unk_584 = unk_584;
            player->m_collidingWithSlopeId = m_collidingWithSlopeId;
            player->m_slopeFlipGravityRelated = m_slopeFlipGravityRelated;
            player->m_slopeAngleRadians = m_slopeAngleRadians;
            player->m_rotationSpeed = m_rotationSpeed;
            player->m_rotateSpeed = m_rotateSpeed;
            player->m_isRotating = m_isRotating;
            player->m_isBallRotating2 = m_isBallRotating2;
            player->m_hasGlow = m_hasGlow;
            player->m_isHidden = m_isHidden;
            player->m_ghostType = m_ghostType;
            player->m_speedMultiplier = m_speedMultiplier;
            player->m_yStart = m_yStart;
            player->m_gravity = m_gravity;
            player->m_trailingParticleLife = m_trailingParticleLife;
            player->m_unk648 = m_unk648;
            player->m_gameModeChangedTime = m_gameModeChangedTime;
            player->m_padRingRelated = m_padRingRelated;
            player->m_maybeReducedEffects = m_maybeReducedEffects;
            player->m_maybeIsFalling = m_maybeIsFalling;
            player->m_shouldTryPlacingCheckpoint = m_shouldTryPlacingCheckpoint;
            player->m_playEffects = m_playEffects;
            player->m_maybeCanRunIntoBlocks = m_maybeCanRunIntoBlocks;
            player->m_hasGroundParticles = m_hasGroundParticles;
            player->m_hasShipParticles = m_hasShipParticles;
            player->m_isOnGround3 = m_isOnGround3;
            player->m_checkpointTimeout = m_checkpointTimeout;
            player->m_lastCheckpointTime = m_lastCheckpointTime;
            player->m_lastJumpTime = m_lastJumpTime;
            player->m_lastFlipTime = m_lastFlipTime;
            player->m_flashTime = m_flashTime;
            player->m_flashRelated = m_flashRelated;
            player->m_flashRelated1 = m_flashRelated1;
            player->m_colorRelated2 = m_colorRelated2;
            player->m_flashRelated3 = m_flashRelated3;
            player->m_lastSpiderFlipTime = m_lastSpiderFlipTime;
            player->m_unkBool5 = m_unkBool5;
            player->m_maybeIsVehicleGlowing = m_maybeIsVehicleGlowing;
            player->m_gv0096 = m_gv0096;
            player->m_gv0100 = m_gv0100;
            player->m_accelerationOrSpeed = m_accelerationOrSpeed;
            player->m_snapDistance = m_snapDistance;
            player->m_ringJumpRelated = m_ringJumpRelated;
            player->m_objectSnappedTo = m_objectSnappedTo;
            player->m_pendingCheckpoint = m_pendingCheckpoint;
            player->m_onFlyCheckpointTries = m_onFlyCheckpointTries;
            player->m_maybeSpriteRelated = m_maybeSpriteRelated;
            player->m_useLandParticles0 = m_useLandParticles0;
            player->m_landParticlesAngle = m_landParticlesAngle;
            player->m_landParticleRelatedY = m_landParticleRelatedY;
            player->m_playerStreak = m_playerStreak;
            player->m_streakRelated1 = m_streakRelated1;
            player->m_streakRelated2 = m_streakRelated2;
            player->m_streakRelated3 = m_streakRelated3;
            player->m_shipStreakType = m_shipStreakType;
            player->m_slopeRotation = m_slopeRotation;
            player->m_currentSlopeYVelocity = m_currentSlopeYVelocity;
            player->m_unk3d0 = m_unk3d0;
            player->m_blackOrbRelated = m_blackOrbRelated;
            player->m_unk3e0 = m_unk3e0;
            player->m_unk3e1 = m_unk3e1;
            player->m_isAccelerating = m_isAccelerating;
            player->m_isCurrentSlopeTop = m_isCurrentSlopeTop;
            player->m_collidedTopMinY = m_collidedTopMinY;
            player->m_collidedBottomMaxY = m_collidedBottomMaxY;
            player->m_collidedLeftMaxX = m_collidedLeftMaxX;
            player->m_collidedRightMinX = m_collidedRightMinX;
            player->m_streakRelated4 = m_streakRelated4;
            // player->m_canPlaceCheckpoint = m_canPlaceCheckpoint;
            player->m_colorRelated = m_colorRelated;
            player->m_secondColorRelated = m_secondColorRelated;
            player->m_hasCustomGlowColor = m_hasCustomGlowColor;
            player->m_glowColor = m_glowColor;
            player->m_maybeIsColliding = m_maybeIsColliding;
            // player->m_jumpBuffered = m_jumpBuffered;
            player->m_stateRingJump = m_stateRingJump;
            player->m_wasJumpBuffered = m_wasJumpBuffered;
            player->m_wasRobotJump = m_wasRobotJump;
            player->m_stateJumpBuffered = m_stateJumpBuffered;
            player->m_stateRingJump2 = m_stateRingJump2;
            player->m_touchedRing = m_touchedRing;
            player->m_touchedCustomRing = m_touchedCustomRing;
            player->m_touchedGravityPortal = m_touchedGravityPortal;
            player->m_maybeTouchedBreakableBlock = m_maybeTouchedBreakableBlock;
            player->m_jumpRelatedAC2 = m_jumpRelatedAC2;
            player->m_touchedPad = m_touchedPad;
            player->m_yVelocity = m_yVelocity;
            player->m_fallSpeed = m_fallSpeed;
            player->m_isOnSlope = m_isOnSlope;
            player->m_wasOnSlope = m_wasOnSlope;
            player->m_slopeVelocity = m_slopeVelocity;
            player->m_maybeUpsideDownSlope = m_maybeUpsideDownSlope;
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
            player->m_reverseRelated = m_reverseRelated;
            player->m_maybeReverseSpeed = m_maybeReverseSpeed;
            player->m_maybeReverseAcceleration = m_maybeReverseAcceleration;
            player->m_xVelocityRelated2 = m_xVelocityRelated2;
            player->m_isDashing = m_isDashing;
            player->m_unk9e8 = m_unk9e8;
            player->m_groundObjectMaterial = m_groundObjectMaterial;
            player->m_vehicleSize = m_vehicleSize;
            player->m_playerSpeed = m_playerSpeed;
            player->m_shipRotation = m_shipRotation;
            player->m_lastPortalPos = m_lastPortalPos;
            player->m_unkUnused3 = m_unkUnused3;
            player->m_isOnGround2 = m_isOnGround2;
            player->m_lastLandTime = m_lastLandTime;
            player->m_platformerVelocityRelated = m_platformerVelocityRelated;
            player->m_maybeIsBoosted = m_maybeIsBoosted;
            player->m_scaleXRelatedTime = m_scaleXRelatedTime;
            player->m_decreaseBoostSlide = m_decreaseBoostSlide;
            player->m_unkA29 = m_unkA29;
            player->m_isLocked = m_isLocked;
            player->m_controlsDisabled = m_controlsDisabled;
            player->m_lastGroundedPos = m_lastGroundedPos;
            player->m_lastActivatedPortal = m_lastActivatedPortal;
            player->m_hasEverJumped = m_hasEverJumped;
            player->m_ringOrStreakRelated = m_ringOrStreakRelated;
            player->m_playerColor1 = m_playerColor1;
            player->m_playerColor2 = m_playerColor2;
            player->m_position = m_position;
            player->m_isSecondPlayer = m_isSecondPlayer;
            player->m_unkA99 = m_unkA99;
            player->m_totalTime = m_totalTime;
            player->m_isBeingSpawnedByDualPortal = m_isBeingSpawnedByDualPortal;
            player->m_unkAAC = m_unkAAC;
            player->m_unkAngle1 = m_unkAngle1;
            player->m_yVelocityRelated3 = m_yVelocityRelated3;
            player->m_gamevar0060 = m_gamevar0060;
            player->m_swapColors = m_swapColors;
            player->m_gamevar0062 = m_gamevar0062;
            player->m_followRelated = m_followRelated;
            player->m_playerFollowFloats = m_playerFollowFloats;
            player->m_unk838 = m_unk838;
            player->m_stateOnGround = m_stateOnGround;
            player->m_stateUnk = m_stateUnk;
            player->m_stateNoStickX = m_stateNoStickX;
            player->m_stateNoStickY = m_stateNoStickY;
            player->m_stateUnk2 = m_stateUnk2;
            player->m_stateBoostX = m_stateBoostX;
            player->m_stateBoostY = m_stateBoostY;
            player->m_maybeStateForce2 = m_maybeStateForce2;
            player->m_stateScale = m_stateScale;
            player->m_platformerXVelocity = m_platformerXVelocity;
            player->m_holdingRight = m_holdingRight;
            player->m_holdingLeft = m_holdingLeft;
            player->m_leftPressedFirst = m_leftPressedFirst;
            player->m_scaleXRelated = m_scaleXRelated;
            player->m_maybeHasStopped = m_maybeHasStopped;
            player->m_xVelocityRelated = m_xVelocityRelated;
            player->m_maybeGoingCorrectSlopeDirection = m_maybeGoingCorrectSlopeDirection;
            player->m_isSliding = m_isSliding;
            player->m_maybeSlopeForce = m_maybeSlopeForce;
            player->m_isOnIce = m_isOnIce;
            player->m_physDeltaRelated = m_physDeltaRelated;
            player->m_isOnGround4 = m_isOnGround4;
            player->m_maybeSlidingTime = m_maybeSlidingTime;
            player->m_maybeSlidingStartTime = m_maybeSlidingStartTime;
            player->m_changedDirectionsTime = m_changedDirectionsTime;
            player->m_slopeEndTime = m_slopeEndTime;
            player->m_isMoving = m_isMoving;
            player->m_platformerMovingLeft = m_platformerMovingLeft;
            player->m_platformerMovingRight = m_platformerMovingRight;
            player->m_isSlidingRight = m_isSlidingRight;
            player->m_maybeChangedDirectionAngle = m_maybeChangedDirectionAngle;
            player->m_unkUnused2 = m_unkUnused2;
            player->m_isPlatformer = m_isPlatformer;
            player->m_stateNoAutoJump = m_stateNoAutoJump;
            player->m_stateDartSlide = m_stateDartSlide;
            player->m_stateHitHead = m_stateHitHead;
            player->m_stateFlipGravity = m_stateFlipGravity;
            player->m_gravityMod = m_gravityMod;
            player->m_stateForce = m_stateForce;
            player->m_stateForceVector = m_stateForceVector;
            player->m_affectedByForces = m_affectedByForces;
            player->m_jumpPadRelated = m_jumpPadRelated;
            player->m_somethingPlayerSpeedTime = m_somethingPlayerSpeedTime;
            player->m_playerSpeedAC = m_playerSpeedAC;
            player->m_fixRobotJump = m_fixRobotJump;
            player->m_inputsLocked = m_inputsLocked;
            player->m_currentRobotAnimation = m_currentRobotAnimation;
            player->m_gv0123 = m_gv0123;
            player->m_iconRequestID = m_iconRequestID;
            player->m_unkUnused = m_unkUnused;
            player->m_isOutOfBounds = m_isOutOfBounds;
            player->m_fallStartY = m_fallStartY;
            player->m_disablePlayerSqueeze = m_disablePlayerSqueeze;
            player->m_robotHasRun3 = m_robotHasRun3;
            player->m_robotHasRun2 = m_robotHasRun2;
            player->m_item20 = m_item20;
            player->m_ignoreDamage = m_ignoreDamage;
            player->m_enable22Changes = m_enable22Changes;

            player->setPositionX(m_xPosition);
            player->setPositionY(m_yPosition);
            player->setRotation(m_rotation);

#ifndef GEODE_IS_ANDROID
            player->m_rotateObjectsRelated = m_rotateObjectsRelated;
            player->m_maybeRotatedObjectsMap = m_maybeRotatedObjectsMap;
            player->m_ringRelatedSet = m_ringRelatedSet;
            player->m_touchedRings = m_touchedRings;
#else
            std::copy(std::begin(m_rotateObjectsRelated), std::end(m_rotateObjectsRelated), std::begin(player->m_rotateObjectsRelated));
            std::copy(std::begin(m_maybeRotatedObjectsMap), std::end(m_maybeRotatedObjectsMap), std::begin(player->m_maybeRotatedObjectsMap));
            std::copy(std::begin(m_ringRelatedSet), std::end(m_ringRelatedSet), std::begin(player->m_ringRelatedSet));
            std::copy(std::begin(m_touchedRings), std::end(m_touchedRings), std::begin(player->m_touchedRings));
#endif
        }

    private:
        float m_xPosition;
        float m_yPosition;
        float m_rotation;

        bool m_wasTeleported;
        bool m_fixGravityBug;
        bool m_reverseSync;
        double m_yVelocityBeforeSlope;
        double m_dashX;
        double m_dashY;
        double m_dashAngle;
        double m_dashStartTime;
        double m_slopeStartTime;
        bool m_justPlacedStreak;
        GameObject* m_maybeLastGroundObject;
        int m_lastCollisionBottom;
        int m_lastCollisionTop;
        int m_lastCollisionLeft;
        int m_lastCollisionRight;
        int m_unk50C;
        int m_unk510;
        GameObject* m_currentSlope2;
        GameObject* m_preLastGroundObject;
        float m_slopeAngle;
        bool m_slopeSlidingMaybeRotated;
        bool m_quickCheckpointMode;
        GameObject* m_collidedObject;
        GameObject* m_lastGroundObject;
        GameObject* m_collidingWithLeft;
        GameObject* m_collidingWithRight;
        int m_maybeSavedPlayerFrame;
        double m_scaleXRelated2;
        double m_groundYVelocity;
        double m_yVelocityRelated;
        double m_scaleXRelated3;
        double m_scaleXRelated4;
        double m_scaleXRelated5;
        bool m_isCollidingWithSlope;
        bool m_isBallRotating;
        bool m_unk669;
        GameObject* m_currentSlope3;
        GameObject* m_currentSlope;
        double unk_584;
        int m_collidingWithSlopeId;
        bool m_slopeFlipGravityRelated;
        float m_slopeAngleRadians;
        gd::unordered_map<int, GJPointDouble> m_rotateObjectsRelated;
        gd::unordered_map<int, GameObject*> m_maybeRotatedObjectsMap;
        float m_rotationSpeed;
        float m_rotateSpeed;
        bool m_isRotating;
        bool m_isBallRotating2;
        bool m_hasGlow;
        bool m_isHidden;
        GhostType m_ghostType;
        double m_speedMultiplier;
        double m_yStart;
        double m_gravity;
        float m_trailingParticleLife;
        float m_unk648;
        double m_gameModeChangedTime;
        bool m_padRingRelated;
        bool m_maybeReducedEffects;
        bool m_maybeIsFalling;
        bool m_shouldTryPlacingCheckpoint;
        bool m_playEffects;
        bool m_maybeCanRunIntoBlocks;
        bool m_hasGroundParticles;
        bool m_hasShipParticles;
        bool m_isOnGround3;
        bool m_checkpointTimeout;
        double m_lastCheckpointTime;
        double m_lastJumpTime;
        double m_lastFlipTime;
        double m_flashTime;
        float m_flashRelated;
        float m_flashRelated1;
        cocos2d::ccColor3B m_colorRelated2;
        cocos2d::ccColor3B m_flashRelated3;
        double m_lastSpiderFlipTime;
        bool m_unkBool5;
        bool m_maybeIsVehicleGlowing;
        bool m_gv0096;
        bool m_gv0100;
        double m_accelerationOrSpeed;
        double m_snapDistance;
        bool m_ringJumpRelated;
        gd::unordered_set<int> m_ringRelatedSet;
        GameObject* m_objectSnappedTo;
        CheckpointObject* m_pendingCheckpoint;
        int m_onFlyCheckpointTries;
        bool m_maybeSpriteRelated;
        bool m_useLandParticles0;
        float m_landParticlesAngle;
        float m_landParticleRelatedY;
        int m_playerStreak;
        float m_streakRelated1;
        bool m_streakRelated2;
        bool m_streakRelated3;
        ShipStreak m_shipStreakType;
        double m_slopeRotation;
        double m_currentSlopeYVelocity;
        double m_unk3d0;
        double m_blackOrbRelated;
        bool m_unk3e0;
        bool m_unk3e1;
        bool m_isAccelerating;
        bool m_isCurrentSlopeTop;
        double m_collidedTopMinY;
        double m_collidedBottomMaxY;
        double m_collidedLeftMaxX;
        double m_collidedRightMinX;
        bool m_streakRelated4;
        // bool m_canPlaceCheckpoint;
        cocos2d::ccColor3B m_colorRelated;
        cocos2d::ccColor3B m_secondColorRelated;
        bool m_hasCustomGlowColor;
        cocos2d::ccColor3B m_glowColor;
        bool m_maybeIsColliding;
        // bool m_jumpBuffered;
        bool m_stateRingJump;
        bool m_wasJumpBuffered;
        bool m_wasRobotJump;
        unsigned char m_stateJumpBuffered;
        bool m_stateRingJump2;
        bool m_touchedRing;
        bool m_touchedCustomRing;
        bool m_touchedGravityPortal;
        bool m_maybeTouchedBreakableBlock;
        geode::SeedValueRSV m_jumpRelatedAC2;
        bool m_touchedPad;
        double m_yVelocity;
        double m_fallSpeed;
        bool m_isOnSlope;
        bool m_wasOnSlope;
        float m_slopeVelocity;
        bool m_maybeUpsideDownSlope;
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
        int m_reverseRelated;
        double m_maybeReverseSpeed;
        double m_maybeReverseAcceleration;
        float m_xVelocityRelated2;
        bool m_isDashing;
        int m_unk9e8;
        int m_groundObjectMaterial;
        float m_vehicleSize;
        float m_playerSpeed;
        cocos2d::CCPoint m_shipRotation;
        cocos2d::CCPoint m_lastPortalPos;
        float m_unkUnused3;
        bool m_isOnGround2;
        double m_lastLandTime;
        float m_platformerVelocityRelated;
        bool m_maybeIsBoosted;
        double m_scaleXRelatedTime;
        bool m_decreaseBoostSlide;
        bool m_unkA29;
        bool m_isLocked;
        bool m_controlsDisabled;
        cocos2d::CCPoint m_lastGroundedPos;
        gd::unordered_set<int> m_touchedRings;
        GameObject* m_lastActivatedPortal;
        bool m_hasEverJumped;
        bool m_ringOrStreakRelated;
        cocos2d::ccColor3B m_playerColor1;
        cocos2d::ccColor3B m_playerColor2;
        cocos2d::CCPoint m_position;
        bool m_isSecondPlayer;
        bool m_unkA99;
        double m_totalTime;
        bool m_isBeingSpawnedByDualPortal;
        float m_unkAAC;
        float m_unkAngle1;
        float m_yVelocityRelated3;
        bool m_gamevar0060;
        bool m_swapColors;
        bool m_gamevar0062;
        int m_followRelated;
        gd::vector<float> m_playerFollowFloats;
        float m_unk838;
        int m_stateOnGround;
        unsigned char m_stateUnk;
        unsigned char m_stateNoStickX;
        unsigned char m_stateNoStickY;
        unsigned char m_stateUnk2;
        int m_stateBoostX;
        int m_stateBoostY;
        int m_maybeStateForce2;
        int m_stateScale;
        double m_platformerXVelocity;
        bool m_holdingRight;
        bool m_holdingLeft;
        bool m_leftPressedFirst;
        double m_scaleXRelated;
        bool m_maybeHasStopped;
        float m_xVelocityRelated;
        bool m_maybeGoingCorrectSlopeDirection;
        bool m_isSliding;
        double m_maybeSlopeForce;
        bool m_isOnIce;
        double m_physDeltaRelated;
        bool m_isOnGround4;
        int m_maybeSlidingTime;
        double m_maybeSlidingStartTime;
        double m_changedDirectionsTime;
        double m_slopeEndTime;
        bool m_isMoving;
        bool m_platformerMovingLeft;
        bool m_platformerMovingRight;
        bool m_isSlidingRight;
        double m_maybeChangedDirectionAngle;
        double m_unkUnused2;
        bool m_isPlatformer;
        int m_stateNoAutoJump;
        int m_stateDartSlide;
        int m_stateHitHead;
        int m_stateFlipGravity;
        float m_gravityMod;
        int m_stateForce;
        cocos2d::CCPoint m_stateForceVector;
        bool m_affectedByForces;
        gd::map<int, bool> m_jumpPadRelated;
        float m_somethingPlayerSpeedTime;
        float m_playerSpeedAC;
        bool m_fixRobotJump;
        // gd::map<int, bool> m_holdingButtons;
        bool m_inputsLocked;
        gd::string m_currentRobotAnimation;
        bool m_gv0123;
        int m_iconRequestID;
        int m_unkUnused;
        bool m_isOutOfBounds;
        float m_fallStartY;
        bool m_disablePlayerSqueeze;
        bool m_robotHasRun3;
        bool m_robotHasRun2;
        bool m_item20;
        bool m_ignoreDamage;
        bool m_enable22Changes;
    };

    class CheckpointData {
    public:
        CheckpointData() = default;

        CheckpointData(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1 = FixPlayerCheckpoint(player1);
            if (player2)
                m_checkpointPlayer2 = FixPlayerCheckpoint(player2);
        }

        void apply(PlayerObject* player1, PlayerObject* player2) {
            m_checkpointPlayer1.apply(player1);
            if (player2)
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
            if (m_checkpointArray->count() <= 0)
                m_fields->m_checkpoints.clear();

            PlayLayer::resetLevel();
        }

        void loadFromCheckpoint(CheckpointObject* checkpoint) {
            auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get());

            if (PracticeFix::shouldEnable() && playLayer->m_fields->m_checkpoints.contains(checkpoint)) {
                PlayLayer::loadFromCheckpoint(checkpoint);

                CheckpointData& data = playLayer->m_fields->m_checkpoints[checkpoint];
                data.apply(playLayer->m_player1, playLayer->m_gameState.m_isDualMode ? playLayer->m_player2 : nullptr);

                return;
            }

            PlayLayer::loadFromCheckpoint(checkpoint);
        }

    };

    class $modify(PracticeFixLELHook, LevelEditorLayer) {
        bool init(GJGameLevel* level, bool unk) {
            bool result = LevelEditorLayer::init(level, unk);

            if (auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get()))
                playLayer->m_fields->m_checkpoints.clear();
            
            return result;
        }
    };

    class $modify(PracticeFixCOHook, CheckpointObject) {
#ifdef GEODE_IS_ANDROID
        static CheckpointObject* create() { // this is so dumb
            auto result = CheckpointObject::create();
#else 
        bool init() override {
            auto result = CheckpointObject::init();
#endif

            if (!PracticeFix::shouldEnable())
                return result;

            auto* playLayer = static_cast<FixPlayLayer*>(FixPlayLayer::get());

            if (playLayer->m_gameState.m_currentProgress > 0) {
                CheckpointData data(playLayer->m_player1, playLayer->m_gameState.m_isDualMode ? playLayer->m_player2 : nullptr);
#ifdef GEODE_IS_ANDROID
                playLayer->m_fields->m_checkpoints[result] = data;
#else
                playLayer->m_fields->m_checkpoints[this] = data;
#endif
            }

            return result;
        }
    };

}
