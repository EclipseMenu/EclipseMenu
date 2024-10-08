#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/HardStreak.hpp>

namespace eclipse::hacks::Player {

    class CustomWaveTrail : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Player");

            config::setIfEmpty("player.customwavetrail.scale", 2.f);
            config::setIfEmpty("player.customwavetrail.speed", 0.5f);
            config::setIfEmpty("player.customwavetrail.saturation", 100.f);
            config::setIfEmpty("player.customwavetrail.value", 100.f);
            config::setIfEmpty("player.customwavetrail.color", gui::Color::WHITE);

            tab->addToggle("Custom Wave Trail", "player.customwavetrail")
                ->handleKeybinds()
                ->setDescription("Customize the wave trail color and size")
                ->addOptions([](auto options) {
                    options->addInputFloat("Scale", "player.customwavetrail.scale", 0.f, 10.f, "%.2f");
                    options->addToggle("Rainbow", "player.customwavetrail.rainbow")->addOptions([](auto opt) {
                        opt->addInputFloat("Speed", "player.customwavetrail.speed", 0.f, FLT_MAX, "%.2f");
                        opt->addInputFloat("Saturation", "player.customwavetrail.saturation", 0.f, 100.f, "%.2f");
                        opt->addInputFloat("Value", "player.customwavetrail.value", 0.f, 100.f, "%.2f");
                    });
                    options->addToggle("Custom color", "player.customwavetrail.customcolor")->addOptions([](auto opt) {
                        opt->addColorComponent("Color", "player.customwavetrail.color");
                    });
                });

            tab->addFloatToggle("Wave Trail Size", "player.wavetrailsize", 0.f, 3.f, "%.2f");
        }

        [[nodiscard]] const char* getId() const override { return "Custom Wave Trail"; }
    };

    REGISTER_HACK(CustomWaveTrail)

    class $modify(WaveTrailSizeHSHook, HardStreak) {
        ADD_HOOKS_DELEGATE("player.customwavetrail")

        void updateStroke(float dt) {
            if (config::get<bool>("player.customwavetrail.rainbow", false)) {
                auto speed = config::get<float>("player.customwavetrail.speed", 0.5f);
                auto saturation = config::get<float>("player.customwavetrail.saturation", 100.f);
                auto value = config::get<float>("player.customwavetrail.value", 100.f);
                this->setColor(utils::getRainbowColor(speed / 10.f, saturation / 100.f, value / 100.f).toCCColor3B());
            } else if (config::get<bool>("player.customwavetrail.customcolor", false)) {
                auto color = config::get<gui::Color>("player.customwavetrail.color", gui::Color::WHITE);
                this->setColor(color.toCCColor3B());
            }

            this->m_pulseSize = config::get<float>("player.customwavetrail.scale", 2.f);

            HardStreak::updateStroke(dt);
        }
    };

}