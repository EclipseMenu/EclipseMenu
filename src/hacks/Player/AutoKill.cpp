#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Player {

    class AutoKill : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty("player.autockill.toggle", false);
            config::setIfEmpty("player.autokill", 50.f);

            tab->addFloatToggle("Auto Kill", "player.autokill", 0.f, 100.f, "%.2f%");
        }

        [[nodiscard]] const char* getId() const override { return "Auto Kill"; }
    };

    REGISTER_HACK(AutoKill)

    class $modify(GJBaseGameLayer) {
        virtual void update(float p0) {
            auto *playLayer = PlayLayer::get();
            if (config::get<bool>("player.autokill.toggle", false)) {
                auto killPercent = config::get<float>("player.autokill");
                float currentPercent = playLayer->getCurrentPercent();
                if (currentPercent >= killPercent && playLayer != nullptr && playLayer->m_player1->m_isPlatformer == false) {
                    playLayer->resetLevel();
                }
            }
            GJBaseGameLayer::update(p0);
        }
    };

}
