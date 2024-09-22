#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/variables.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class Noclip : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty("player.noclip.opacity", 90.f);
            config::setIfEmpty("player.noclip.time", 0.f);
            config::setIfEmpty("player.noclip.color", gui::Color::RED);

            tab->addToggle("Noclip", "player.noclip")
                ->setDescription("Disables player death")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addToggle("Noclip Tint", "player.noclip.tint");
                    options->addColorComponent("Tint Color", "player.noclip.color");
                    options->addInputFloat("Tint Opacity", "player.noclip.opacity", 0.f, 100.f, "%.0f%");
                    options->addInputFloat("Tint Time", "player.noclip.time", 0.f, 5.f, "%.2fs")->setDescription("0 for instant");
                });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("player.noclip", false); }
        [[nodiscard]] const char* getId() const override { return "Noclip"; }
    };

    REGISTER_HACK(Noclip)

    class $modify(NoClipPLHook, PlayLayer) {
        struct Fields {
            cocos2d::CCLayerColor* m_noclipTint = nullptr;
            bool m_wouldDie = false;
            bool m_wouldDieFrame = false;
            bool m_deadLastFrame = false;
            float m_tintTimer = 0.f;
            float m_tintOpacity = 0.f;
            size_t m_deadFrames = 0;
        };

        ENABLE_SAFE_HOOKS_ALL()

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            if (object == m_anticheatSpike)
                return PlayLayer::destroyPlayer(player, object);

            auto& fields = m_fields;

            if (!fields->m_noclipTint && config::get<bool>("player.noclip.tint", false)) {
                auto color = config::get<gui::Color>("player.noclip.color", gui::Color::RED).toCCColor3B();
                fields->m_noclipTint = cocos2d::CCLayerColor::create(
                    cocos2d::_ccColor4B {color.r, color.g, color.b, 0}
                );
                fields->m_noclipTint->setZOrder(1000);
                fields->m_noclipTint->setID("nocliptint"_spr);
                if (auto uiMenu = geode::cast::typeinfo_cast<cocos2d::CCMenu*>(m_uiLayer->getChildByID("eclipse-ui"_spr))) {
                    uiMenu->addChild(fields->m_noclipTint);
                } else { // fallback
                    m_uiLayer->addChild(fields->m_noclipTint);
                }
            }

            if (config::get<bool>("player.noclip", false)) {
                fields->m_wouldDieFrame = true;
                fields->m_wouldDie = true;
                fields->m_tintTimer = 0.f;
            }

            if (!config::get<bool>("player.noclip", false))
                PlayLayer::destroyPlayer(player, object);
        }

        void postUpdate(float dt) override {
            auto& fields = m_fields;
            if (config::get<bool>("player.noclip.tint", false) && fields->m_noclipTint && !m_hasCompletedLevel && !m_player1->m_isDead) {
                float time = config::get<float>("player.noclip.time", 0.f);
                if (time == 0.f) { // this doesnt really work but ok ninx
                    if (fields->m_wouldDie) {
                        if (fields->m_tintOpacity < 1.f) {
                            fields->m_tintOpacity += 0.25f;
                            fields->m_noclipTint->setOpacity(fields->m_tintOpacity * config::get<float>("player.noclip.opacity", 90.f));
                        }
                        fields->m_wouldDie = false;
                    } else {
                        if (fields->m_tintOpacity > 0.f) {
                            fields->m_tintOpacity -= 0.25f;
                            fields->m_noclipTint->setOpacity(fields->m_tintOpacity * config::get<float>("player.noclip.opacity", 90.f));
                        }
                    }
                } else {
                    if (fields->m_wouldDie) {
                        fields->m_tintTimer += dt;
                        float progress = fields->m_tintTimer / time;
                        if (progress >= 1.0f) {
                            fields->m_noclipTint->setOpacity(0);
                            fields->m_wouldDie = false;
                            fields->m_tintTimer = 0.F;
                            fields->m_tintOpacity = 0.f;
                        } else {
                            auto startOpacity = config::get<float>("player.noclipflash.opacity", 90.f);
                            fields->m_tintOpacity = (255.F * (startOpacity / 100.F)) * (1.f - progress);
                        }
                        if (fields->m_tintOpacity <= 0.0F)
                            fields->m_tintOpacity = 0.0F;
                        fields->m_noclipTint->setOpacity(m_fields->m_tintOpacity);
                    }
                }
            }

            PlayLayer::postUpdate(dt);
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            auto& fields = m_fields;
            fields->m_wouldDie = false;
            fields->m_wouldDieFrame = false;
            fields->m_deadFrames = 0;
            config::setTemp<int>("noclipDeaths", 0);
        }
    };

    class $modify(NoClipGJBGLHook, GJBaseGameLayer) {
        void processCommands(float dt) {
            GJBaseGameLayer::processCommands(dt);

            if (!PlayLayer::get()) {
                config::setTemp<int>("noclipDeaths", 0);
                config::setTemp<float>("noclipAccuracy", 100.f);
                return;
            }

            auto pl = reinterpret_cast<NoClipPLHook*>(this);
            if (pl->m_hasCompletedLevel || pl->m_levelEndAnimationStarted) return;

            auto fields = pl->m_fields.self();
            if (fields->m_wouldDieFrame) {
                fields->m_deadFrames++;
                if (!fields->m_deadLastFrame) {
                    auto deaths = config::getTemp<int>("noclipDeaths", 0);
                    config::setTemp<int>("noclipDeaths", deaths + 1);
                }
            }

            fields->m_deadLastFrame = fields->m_wouldDieFrame;
            fields->m_wouldDieFrame = false;

            auto frame = m_gameState.m_currentProgress;
            if (frame > 0) {
                float acc = static_cast<float>(frame - fields->m_deadFrames) / static_cast<float>(frame) * 100.f;
                config::setTemp("noclipAccuracy", acc);
            }
        }
    };

}
