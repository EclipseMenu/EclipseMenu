#include "float-button.hpp"
#include <modules/config/config.hpp>
#include <modules/utils/SingletonCache.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/CCScene.hpp>
using namespace cocos2d;

namespace eclipse::gui {
    FloatingButton* FloatingButton::get() {
        static FloatingButton* instance = create();
        return instance;
    }

    void FloatingButton::setCallback(Function<void()>&& callback) {
        m_callback = std::move(callback);
    }

    void FloatingButton::setScale(float scale) {
        m_baseScale = scale;
        m_sprite->setScale(scale);
    }

    void FloatingButton::reloadSprite() {
        this->removeChild(m_sprite, true);
        m_sprite = createSprite();
        this->addChild(m_sprite);
    }

    FloatingButton* FloatingButton::create() {
        auto ret = new FloatingButton();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }
        delete ret;
        return nullptr;
    }

    cocos2d::CCSprite* FloatingButton::createSprite() const {
        auto spr = CCSprite::create("ECLIPSE-android.png"_spr);
        spr->setScale(config::get<float>("float-btn.scale", 0.25f));
        spr->setOpacity(m_minOpacity * 255);
        spr->setPosition({
            config::get<float>("float-btn.x", 480.f),
            config::get<float>("float-btn.y", 200.f)
        });
        spr->setID("main-sprite");
        return spr;
    }

    bool FloatingButton::init() {
        if (!CCMenu::init())
            return false;

        // setup settings
        m_baseScale = config::get<double>("float-btn.scale", 0.25f);
        m_maxOpacity = config::get<double>("float-btn.max-opacity", 1.f);
        m_minOpacity = config::get<double>("float-btn.min-opacity", 0.5f);
        m_showInLevel = config::get<bool>("float-btn.show-in-level", false);
        m_showInEditor = config::get<bool>("float-btn.show-in-editor", true);

        // add delegates
        config::addDelegate("float-btn.max-opacity", [this] {
            m_maxOpacity = config::get<double>("float-btn.max-opacity", 1.f);
        });
        config::addDelegate("float-btn.min-opacity", [this] {
            m_minOpacity = config::get<double>("float-btn.min-opacity", 0.5f);
        });
        config::addDelegate("float-btn.show-in-level", [this] {
            m_showInLevel = config::get<bool>("float-btn.show-in-level", false);
        });
        config::addDelegate("float-btn.show-in-editor", [this] {
            m_showInEditor = config::get<bool>("float-btn.show-in-editor", true);
        });
        config::addDelegate("float-btn.scale", [this] {
            this->setScale(config::get<double>("float-btn.scale", 0.25f));
        });

        // setup button
        this->setZOrder(256);
        this->setPosition({0, 0});
        this->setID("floating-button"_spr);
        this->scheduleUpdate();

        m_sprite = createSprite();
        this->addChild(m_sprite);

        geode::OverlayManager::get()->addChild(this);

        // im mostly sure this will override the next priorities, so i guess this should not be a force prio
        // utils::get<cocos2d::CCTouchDispatcher>()->registerForcePrio(this, 2);

        return true;
    }

    void FloatingButton::update(float) {
        this->setVisible(!shouldHide());

        auto dt = utils::get<CCDirector>()->getActualDeltaTime();
        if (m_haveReleased) {
            m_postClickTimer -= dt;
            if (m_postClickTimer <= 0) {
                m_postClickTimer = 0;
                m_haveReleased = false;
                this->fadeOut();
            }
        }

        auto currentPos = m_sprite->getPosition();
        auto radius = getRadius();
        auto winSize = utils::get<CCDirector>()->getWinSize();

        auto clampedPos = currentPos;
        clampedPos.x = std::clamp(currentPos.x, radius, winSize.width - radius);
        clampedPos.y = std::clamp(currentPos.y, radius, winSize.height - radius);

        if (currentPos.x != clampedPos.x || currentPos.y != clampedPos.y) {
            m_sprite->setPosition(clampedPos);
            config::set<float>("float-btn.x", clampedPos.x);
            config::set<float>("float-btn.y", clampedPos.y);
            currentPos = clampedPos;
        }

        if (!m_shouldMove) return;

        // if very close to the end, speed up
        if (ccpDistance(currentPos, m_holdPosition) < SNAP_MARGIN * 10.f) {
            dt *= 2.f;
        }

        auto newPoint = ccpLerp(currentPos, m_holdPosition, MOVE_SPEED * dt);
        if (ccpDistance(newPoint, m_holdPosition) < SNAP_MARGIN) {
            newPoint = m_holdPosition;
            m_shouldMove = false; // finished moving
        }

        newPoint.x = std::clamp(newPoint.x, radius, winSize.width - radius);
        newPoint.y = std::clamp(newPoint.y, radius, winSize.height - radius);

        this->m_sprite->setPosition(newPoint);

        config::set<float>("float-btn.x", newPoint.x);
        config::set<float>("float-btn.y", newPoint.y);
    }

    bool FloatingButton::shouldHide() const {
        if (auto pl = utils::get<PlayLayer>(); !m_showInLevel && pl)
            return !pl->m_isPaused && !pl->m_hasCompletedLevel;

        if (auto le = utils::get<LevelEditorLayer>(); !m_showInEditor && le)
            return !le->m_editorUI->m_isPaused;

        return false;
    }

    float FloatingButton::getRadius() const {
        return m_sprite->getScaledContentSize().width / 2.f;
    }

    void FloatingButton::fadeIn() const {
        m_sprite->stopActionByTag(2);
        auto action = CCFadeTo::create(0.2f, m_maxOpacity * 255);
        action->setTag(2);
        m_sprite->runAction(action);
    }

    void FloatingButton::fadeOut() const {
        m_sprite->stopActionByTag(2);
        auto action = CCSequence::create(
            CCDelayTime::create(1.f),
            CCFadeTo::create(0.2f, m_minOpacity * 255),
            nullptr
        );
        action->setTag(2);
        m_sprite->runAction(action);
    }

    void FloatingButton::scaleDown() {
        m_sprite->stopActionByTag(1);
        auto action = CCScaleTo::create(0.1f, m_baseScale * PRESS_SCALE);
        action->setTag(1);
        m_sprite->runAction(action);
    }

    void FloatingButton::scaleUp() {
        m_sprite->stopActionByTag(1);
        auto action = CCEaseBackOut::create(CCScaleTo::create(0.15f, m_baseScale));
        action->setTag(1);
        m_sprite->runAction(action);
    }

    bool FloatingButton::ccTouchBegan(CCTouch* touch, CCEvent* event) {
        if (shouldHide()) return false;

        auto touchPos = this->convertToNodeSpace(touch->getLocation());
        auto spritePos = m_sprite->getPosition();
        if (ccpDistance(touchPos, spritePos) > getRadius())
            return false;

        m_haveMoved = false;
        m_haveReleased = false;
        m_holdPosition = touchPos;
        this->fadeIn();
        this->scaleDown();

        return true;
    }

    void FloatingButton::ccTouchEnded(CCTouch* touch, CCEvent* event) {
        this->scaleUp();

        if (m_haveMoved) {
            this->fadeOut();
            return;
        }

        m_haveReleased = true;
        m_postClickTimer = 1.f;
        if (m_callback) m_callback();
        else geode::log::warn("FloatingButton callback unspecified!");
    }

    void FloatingButton::ccTouchMoved(CCTouch* touch, CCEvent* event) {
        auto touchPos = this->convertToNodeSpace(touch->getLocation());
        if (!m_shouldMove && ccpDistance(touchPos, m_holdPosition) < MIN_MOVE_DISTANCE)
            return;

        m_shouldMove = true;
        m_haveMoved = true;
        m_holdPosition = touchPos;
    }

    void FloatingButton::registerWithTouchDispatcher() {
        utils::get<CCTouchDispatcher>()->addTargetedDelegate(this, -1000, true);
    }

    FloatingButton::~FloatingButton() = default;

// #ifdef ECLIPSE_USE_FLOATING_BUTTON
//     class $modify(CCScene) {
//         /// Allows our button to stay top-most, passing z-order of the node below the button.
//         /// Shout-out to QOLMod for the idea.
//         int getHighestChildZ() {
//             auto btn = FloatingButton::get();
//             auto original = btn->getZOrder();
//             btn->setZOrder(-1);
//
//             auto highest = CCScene::getHighestChildZ();
//             btn->setZOrder(original);
//
//             return highest;
//         }
//     };
// #endif

}
