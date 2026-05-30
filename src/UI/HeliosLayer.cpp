#include <UI/HeliosLayer.hpp>

#include <Geode/modify/CCEGLViewProtocol.hpp>
#include <Geode/modify/CCMouseDispatcher.hpp>

#include <Helios/Helios.hpp>
#include <Helios/Debug/Log.hpp>

using namespace eclipse::prelude;

static void heliosLog(Helios::LogLevel level, std::string_view message) {
    using Helios::LogLevel;
    switch (level) {
        case LogLevel::Trace: log::trace("[Helios] {}", message); break;
        case LogLevel::Debug: log::debug("[Helios] {}", message); break;
        case LogLevel::Info: log::info("[Helios] {}", message); break;
        case LogLevel::Warning: log::warn("[Helios] {}", message); break;
        case LogLevel::Error: log::error("[Helios] {}", message); break;
    }
}

namespace eclipse {
    class TestWidget : public Helios::Widget {
    public:
        void onDraw(Helios::DrawList& dl) override {
            dl.fillRoundedRectGradient(
                {0, 0, m_size.x, m_size.y}, 8.f,
                Helios::Color::red(),
                Helios::Color::green(),
                Helios::Color::blue(),
                m_hovered ?
                    Helios::Color::fromFloats(1.f, 1.f, 0.f, 0.5f) :
                    Helios::Color::transparent()
            );
        }

        void onMouseEnter() override {
            m_hovered = true;
            this->markGeometryDirty();
        }

        void onMouseLeave() override {
            m_hovered = false;
            this->markGeometryDirty();
        }

        bool m_hovered = false;
    };

    static Helios::Vec2 cocosToFrame(CCPoint const& pos) {
        auto frameSize = CCEGLView::get()->m_obScreenSize * utils::getDisplayFactor();
        auto winSize = CCDirector::get()->m_obWinSizeInPoints;
        return Helios::Vec2{
            pos.x / winSize.width * frameSize.width,
            (1.f - pos.y / winSize.height) * frameSize.height
        };
    }

    static Helios::Vec2 getMousePosition() {
        return cocosToFrame(geode::cocos::getMousePos());
    }

    HeliosLayer* HeliosLayer::get() {
        static HeliosLayer* instance = nullptr;
        if (!instance) {
            instance = new HeliosLayer();
            if (!instance->init()) {
                delete instance;
                instance = nullptr;
            }
            instance->autorelease();
        }
        return instance;
    }

    void HeliosLayer::toggle() noexcept {
        this->setToggled(!m_toggled);
    }

    void HeliosLayer::setToggled(bool toggled) noexcept {
        m_toggled = toggled;
        this->setVisible(m_toggled);
    }

    bool HeliosLayer::init() {
        if (!CCLayer::init()) return false;

        auto gl = CCEGLView::get();
        if (!Helios::Director::get().init(
            gl->m_obScreenSize.width,
            gl->m_obScreenSize.height
        )) {
            return false;
        }

        this->setToggled(false);

        Helios::Director::get()
            .setCursorCallback([](Helios::MouseCursor cursor) {
                using Helios::MouseCursor;
                // TODO: set cursor
            });

        this->setID("interface"_spr);

        m_mouseMoveListener = geode::MouseMoveEvent().listen([](int32_t x, int32_t y) {
            Helios::Director::get().handleMouseMoved(getMousePosition());
        });

        m_mouseClickListener = geode::MouseInputEvent().listen([](geode::MouseInputData& data) {
            using namespace geode;
            if (data.button == MouseInputData::Button::Left) return ListenerResult::Propagate;
            return Helios::Director::get().handleMouseButton(
                static_cast<Helios::MouseButton>(data.button),
                data.action == MouseInputData::Action::Press,
                getMousePosition()
            );
        });

        m_keyboardListener = geode::KeyboardInputEvent().listen([](geode::KeyboardInputData& data) {
            // TODO
        });

        this->setTouchEnabled(true);
        this->scheduleUpdate();

        geode::OverlayManager::get()->addChild(this);

        Helios::Director::get()
            .addWidget<TestWidget>()
            ->setRect({10, 10}, {150, 100});

        return true;
    }

    void HeliosLayer::draw() {
        Helios::Director::get().render();
    }

    void HeliosLayer::update(float) {
        // cocos delta time might be speed-hacked, so for accuracy we're counting our own
        static auto lastTime = std::chrono::steady_clock::now();
        auto now = std::chrono::steady_clock::now();
        std::chrono::duration<float> delta = now - lastTime;
        lastTime = now;

        Helios::Director::get().update(delta.count());
    }

    bool HeliosLayer::ccTouchBegan(CCTouch* pTouch, CCEvent* pEvent) {
        if (!m_bVisible) return false;
        #ifdef GEODE_IS_DESKTOP
        return Helios::Director::get().handleMouseButton(
            Helios::MouseButton::Left, true,
            cocosToFrame(pTouch->getLocation())
        );
        #else
        return Helios::Director::get().handleTouchBegin(
            pTouch->getID(),
            cocosToFrame(pTouch->getLocation())
        );
        #endif
    }

    void HeliosLayer::ccTouchMoved(CCTouch* pTouch, CCEvent* pEvent) {
        #ifndef GEODE_IS_DESKTOP
        Helios::Director::get().handleTouchMoved(
            pTouch->getID(),
            cocosToFrame(pTouch->getLocation())
        );
        #endif
    }

    void HeliosLayer::ccTouchEnded(CCTouch* pTouch, CCEvent* pEvent) {
        #ifdef GEODE_IS_DESKTOP
        Helios::Director::get().handleMouseButton(
            Helios::MouseButton::Left, false,
            cocosToFrame(pTouch->getLocation())
        );
        #else
        Helios::Director::get().handleTouchEnded(
            pTouch->getID(),
            cocosToFrame(pTouch->getLocation())
        );
        #endif
    }

    void HeliosLayer::ccTouchCancelled(CCTouch* pTouch, CCEvent* pEvent) {
        #ifdef GEODE_IS_DESKTOP
        Helios::Director::get().handleMouseButton(
            Helios::MouseButton::Left, false,
            cocosToFrame(pTouch->getLocation())
        );
        #else
        Helios::Director::get().handleTouchCancelled(pTouch->getID());
        #endif
    }

    void HeliosLayer::registerWithTouchDispatcher() {
        CCTouchDispatcher::get()->addTargetedDelegate(this, -1000, true);
    }
}

class $modify(HeliosCCEGLVP, CCEGLViewProtocol) {
    void setFrameSize(float width, float height) override {
        CCEGLViewProtocol::setFrameSize(width, height);
        Helios::Director::get().updateViewport(width, height);
    }
};

#ifndef GEODE_IS_IOS
class $modify(HeliosCCMD, CCMouseDispatcher) {
    bool dispatchScrollMSG(float y, float x) {
        static constexpr float scrollMult = 1.f / 10.f;
        Helios::Director::get().handleMouseWheel({x * scrollMult, -y * scrollMult});
        return CCMouseDispatcher::dispatchScrollMSG(y, x);
    }
};
#endif