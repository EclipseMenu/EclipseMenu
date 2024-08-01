#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {

    class NoclipFlash : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            gui::ToggleComponent* toggle = tab->addToggle("Noclip Flash", "player.noclipflash")->handleKeybinds()->setDescription("Flashes the whole screen to a color if you passthrough an object.");
            
            config::setIfEmpty("player.noclipflash.opacity", 90.F);
            config::setIfEmpty("player.noclipflash.time", 0.4F);
            config::setIfEmpty("player.noclipflash.color", gui::Color::RED);
 
            toggle->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                options->addFloatToggle("Flash Time", "player.noclipflash.time", 0.f, 5.f, "%.2fs");
                options->addFloatToggle("Flash Start Opacity", "player.noclipflash.opacity", 0.f, 100.f, "%.0f%");
                options->addColorComponent("Flash Color", "player.noclipflash.color");
            });
        }

        [[nodiscard]] const char* getId() const override { return "Noclip Flash"; }
    };

    REGISTER_HACK(NoclipFlash)

    class $modify(NoClipFlashPLHook, PlayLayer) {
        struct Fields {
            GameObject* m_anticheatObject = nullptr;
            float flashOpacity = 0.0F;
            float flashTimer = 0.0F;
            bool isFlashing = false;
            cocos2d::CCSprite* flashNode;
        };
        bool init(GJGameLevel *level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;
            auto winSize = cocos2d::CCDirector::sharedDirector()->getWinSize();
            m_fields->flashNode = cocos2d::CCSprite::create("square02b_001.png");
            m_fields->flashNode->setScale(10);
            m_fields->flashNode->setPosition(winSize / 2);
            auto color = config::get<gui::Color>("player.noclipflash.color", gui::Color::RED);
            m_fields->flashNode->setColor(color.toCCColor3B());
            m_fields->flashNode->setOpacity(0);
            m_fields->flashNode->setTag(10420);
            m_fields->flashNode->setID("noclip-flash"_spr);
            if (auto uiMenu = geode::cast::typeinfo_cast<cocos2d::CCMenu*>(m_uiLayer->getChildByID("eclipse-ui"_spr))) {
                uiMenu->addChild(m_fields->flashNode, 1000);
            } else { // fallback
                m_uiLayer->addChild(m_fields->flashNode, 1000);
            }
            return true;
        }
        void postUpdate(float dt) override {
            // might not really be optimized for any TPS that is different than 240
            // TODO: fix this to work with any TPS
            if (m_fields->isFlashing && m_fields->flashNode != nullptr) {
                m_fields->flashTimer += dt;
                float progress = m_fields->flashTimer / config::get<float>("player.noclipflash.time", 0.4f);
                if (progress >= 1.0f) {
                    m_fields->flashNode->setOpacity(0);
                    m_fields->flashOpacity = 0.F;
                    m_fields->isFlashing = false;
                    m_fields->flashTimer = 0.F;
                } else {
                    auto startOpacity = config::get<float>("player.noclipflash.opacity", 90.f);
                    m_fields->flashOpacity = (255.F * (startOpacity / 100.F)) * (1.f - progress);
                }
                if (m_fields->flashOpacity <= 0.0F)
                    m_fields->flashOpacity = 0.0F;
                 m_fields->flashNode->setOpacity(m_fields->flashOpacity);
            }
            PlayLayer::postUpdate(dt);
        }
        void resetLevel() {
            if (m_fields->flashNode != nullptr) {
                auto color = config::get<gui::Color>("player.noclipflash.color", gui::Color::RED);
                m_fields->flashNode->setColor(color.toCCColor3B());
                m_fields->flashNode->setOpacity(0);
                m_fields->flashOpacity = 0.F;
                m_fields->isFlashing = false;
                m_fields->flashTimer = 0.F;
            }
            PlayLayer::resetLevel();
        }
        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            if (!m_fields->m_anticheatObject)
                m_fields->m_anticheatObject = object;

            if (object == m_fields->m_anticheatObject)
                return PlayLayer::destroyPlayer(player, object);

            if (config::get<bool>("player.noclipflash", false) && m_fields->flashNode != nullptr) {
                m_fields->flashTimer = 0.0f;
                m_fields->isFlashing = true;
            }
            PlayLayer::destroyPlayer(player, object);
        }
    };

}
