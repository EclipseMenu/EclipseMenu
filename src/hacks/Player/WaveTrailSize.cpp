#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/HardStreak.hpp>

namespace eclipse::hacks::Player {

    class WaveTrailSize : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty("player.wavetrailsize.toggle", false);
            config::setIfEmpty("player.wavetrailsize", 2.f);

            tab->addFloatToggle("Wave Trail Size", "player.wavetrailsize", 0.f, 3.f, "%.2f");
        }

        [[nodiscard]] int32_t getPriority() const override { return 2; }
        [[nodiscard]] const char* getId() const override { return "Wave Trail Size"; }
    };

    REGISTER_HACK(WaveTrailSize)

    class $modify(HardStreak) {
        void updateStroke(float dt) {
            if (config::get<bool>("player.wavetrailsize.toggle", false)) {
                float trailSize = config::get<float>("player.wavetrailsize", 0.f);
                this->m_pulseSize = trailSize;
            }

            HardStreak::updateStroke(dt);
        }
    };

}