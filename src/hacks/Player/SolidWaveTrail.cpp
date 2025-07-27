#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/Geode.hpp>
#include <Geode/modify/PlayerObject.hpp>

namespace eclipse::hacks::Player {
    class $modify(SolidWaveTrailPOHook, PlayerObject) {
        ALL_DELEGATES_AND_SAFE_PRIO("player.solidwavetrail")

        struct Fields {
            std::optional<bool> m_wasSolidOriginally = std::nullopt;
        };

        void toggle(bool enable) {
            auto* fields = m_fields.self();
            if (!fields->m_wasSolidOriginally.has_value()) {
                fields->m_wasSolidOriginally = m_waveTrail->m_isSolid;
            }

            m_waveTrail->m_isSolid = enable || *fields->m_wasSolidOriginally;
            m_waveTrail->setBlendFunc(
                enable ? cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA}
                       : cocos2d::ccBlendFunc{GL_SRC_ALPHA, GL_ONE}
            );
        }

        void setupStreak() {
            PlayerObject::setupStreak();
            this->toggle(true);
        }
    };

    class $hack(SolidWaveTrail) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.player");
            tab->addToggle("player.solidwavetrail")->setDescription()->handleKeybinds();
            config::addDelegate("player.solidwavetrail", []() {
                auto enabled = config::get<"player.solidwavetrail", bool>(false);
                if (auto* gjbgl = utils::get<GJBaseGameLayer>()) {
                    static_cast<SolidWaveTrailPOHook*>(gjbgl->m_player1)->toggle(enabled);
                    static_cast<SolidWaveTrailPOHook*>(gjbgl->m_player2)->toggle(enabled);
                } else if (auto* ml = utils::get<MenuLayer>()) {
                    using namespace geode::prelude;
                    for (auto node : CCArrayExt<CCNode*>(ml->m_menuGameLayer->getChildren())) {
                        if (auto player = typeinfo_cast<PlayerObject*>(node)) {
                            static_cast<SolidWaveTrailPOHook*>(player)->toggle(enabled);
                        }
                    }
                }
            });
        }

        [[nodiscard]] const char* getId() const override { return "Solid Wave Trail"; }
    };

    REGISTER_HACK(SolidWaveTrail)
}
