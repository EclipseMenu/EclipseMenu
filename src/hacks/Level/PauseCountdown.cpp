#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Level {

    class PauseCount : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            config::setIfEmpty("level.pausecount.time", 3.f);

            tab->addToggle("Pause Countdown", "level.pausecount")->addOptions([] (gui::MenuTab* options) {
                options->addInputFloat("Countdown Time", "level.pausecount.time", 0.1f, 15.f, "%.2fs");
            });
        }

        [[nodiscard]] const char* getId() const override { return "Pause Countdown"; }
    };

    REGISTER_HACK(PauseCount)

    class PauseCountdown : public CCLayerColor {
        public:
            static PauseCountdown* create() {
                auto ret = new PauseCountdown;
                if (ret && ret->init()) {
                    ret->autorelease();
                    return ret;
                }
                delete ret;
                return nullptr;
            }
        protected:
            CCLabelBMFont* m_countdown = nullptr;
            float m_totaldt = 3.f;

            bool init() override {
                if (!CCLayerColor::initWithColor(ccc4(50, 50, 50, 100))) return false;
                m_totaldt = config::get<float>("level.pausecount.time", 3.f);
                m_countdown = CCLabelBMFont::create(std::to_string(static_cast<int>(m_totaldt)).c_str(), "goldFont.fnt");
                m_countdown->setScale(1.5);
                m_countdown->setPosition(CCDirector::sharedDirector()->getWinSize().width / 2, CCDirector::sharedDirector()->getWinSize().height / 2);
                addChild(m_countdown, 100);

                this->schedule(schedule_selector(PauseCountdown::updateTimer));

                return true;
            }

            void updateTimer(float dt) {
                m_totaldt -= dt;
                if (m_totaldt <= 0.5f) this->removeFromParent();
                m_countdown->setString(std::to_string(static_cast<int>(m_totaldt) + 1).c_str());
            }
    };

    class $modify(PauseBGL, GJBaseGameLayer) {
        struct Fields {
            float pausedt = 0.f;
        };

        void update(float dt) {
            if (!PlayLayer::get()) return GJBaseGameLayer::update(dt);
            if (m_fields->pausedt > 0.f) m_fields->pausedt -= dt;
            if (m_fields->pausedt <= 0.5f || !config::get<bool>("level.pausecount", false)) {
                GJBaseGameLayer::update(dt);
            }
        }
    };

    class $modify(PauseLayer) {
        void onResume(cocos2d::CCObject* sender) {
            PauseLayer::onResume(sender);

            PauseBGL* bg = static_cast<PauseBGL*>(PauseBGL::get());

            if (!PlayLayer::get() || !bg) return;
            if (bg->m_gameState.m_currentProgress == 0) return;
            bg->m_fields->pausedt = config::get<float>("level.pausecount.time", 3.f);
            if (config::get<bool>("level.pausecount", false)) bg->addChild(PauseCountdown::create());
        }
    };
}
