#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/variables.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {

    class Noclip : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty("player.noclip.opacity", 90.f);

            tab->addToggle("Noclip", "player.noclip")
                ->setDescription("Disables player death")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addToggle("Noclip Tint", "player.noclip.tint");
                    options->addInputFloat("Tint Opacity", "player.noclip.opacity");
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
            float m_tintOpacity = 0.f;
        };

        static void onModify(auto& self) {
            SAFE_PRIORITY("PlayLayer::destroyPlayer");
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            if (object == m_anticheatSpike)
                return PlayLayer::destroyPlayer(player, object);

            auto& fields = m_fields;

            if (!fields->m_noclipTint && config::get<bool>("player.noclip.tint", false)) {
                fields->m_noclipTint = cocos2d::CCLayerColor::create(cocos2d::_ccColor4B {255, 0, 0, 0});
                fields->m_noclipTint->setZOrder(1000);
                m_uiLayer->addChild(fields->m_noclipTint);
            }

            if (config::get<bool>("player.noclip", false))
                fields->m_wouldDie = true;

            if (!config::get<bool>("player.noclip", false))
                PlayLayer::destroyPlayer(player, object);
        }

        void postUpdate(float dt) override {
            auto& fields = m_fields;
            if (config::get<bool>("player.noclip.tint", false) && fields->m_noclipTint && !m_hasCompletedLevel && !m_player1->m_isDead) {
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
            }

            PlayLayer::postUpdate(dt);
        }
    };

}
