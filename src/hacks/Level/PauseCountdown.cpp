#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

namespace eclipse::hacks::Level {
    class PauseCount : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.pausecount.time", 3.f);

            tab->addToggle("level.pausecount")->handleKeybinds()->setDescription()
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addInputFloat("level.pausecount.time", 0.1f, 15.f, "%.2fs");
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
            m_countdown->setPosition(utils::get<CCDirector>()->getWinSize() / 2);
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

    class $modify(PauseBGLHook, GJBaseGameLayer) {
        struct Fields {
            float pausedt = 0.f;
        };

        void update(float dt) {
            if (!utils::get<PlayLayer>()) return GJBaseGameLayer::update(dt);

            if (m_fields->pausedt > 0.f) m_fields->pausedt -= dt;
            if (m_fields->pausedt <= 0.01f || !config::get<bool>("level.pausecount", false))
                GJBaseGameLayer::update(dt);
        }
    };

    class $modify(PauseCountdownPlayLHook, PlayLayer) {
        void pauseGame(bool paused) {
            if (paused) {
                if (static_cast<PauseBGLHook*>(utils::get<GJBaseGameLayer>())->m_fields->pausedt < 0.01f
                    || !config::get<bool>("level.pausecount", false))
                    PlayLayer::pauseGame(paused);
            } else {
                PlayLayer::pauseGame(paused);
                auto* bg = static_cast<PauseBGLHook*>(utils::get<GJBaseGameLayer>());

                if (!utils::get<PlayLayer>() || !bg) return;
                if (bg->m_gameState.m_currentProgress == 0) return;

                bg->m_fields->pausedt = config::get<float>("level.pausecount.time", 3.f);

                if (config::get<bool>("level.pausecount", false))
                    bg->addChild(PauseCountdown::create());
            }
        }
    };
}
