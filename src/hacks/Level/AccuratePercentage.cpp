#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Global {
    class AccuratePercentage : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            config::setIfEmpty("level.accuratepercent.amount", 4);
            config::setIfEmpty("level.accuratepercent.normal_mode", true);
            config::setIfEmpty("level.accuratepercent.bugfix", true);
            config::setIfEmpty("level.accuratepercent.show_minutes", true);

            tab->addToggle("Accurate Percentage", "level.accuratepercentage")
                ->setDescription("Allows for more decimals in a level percentage, and adds other useful utils.")
                ->handleKeybinds()
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addToggle("Normal Mode", "level.accuratepercent.normal_mode");
                    options->addInputInt("Decimal Places", "level.accuratepercent.amount", 0, 15);
                    options->addToggle("Fix 0% bug", "level.accuratepercent.bugfix");
                    options->addToggle("Formatted Timer", "level.accuratepercent.show_minutes");
                });
        }

        [[nodiscard]] const char* getId() const override { return "Accurate Percentage"; }
    };

    REGISTER_HACK(AccuratePercentage)

    class $modify(AccuratePercentagePLHook, PlayLayer) {
        float customGetProgress() {
            if (config::get<bool>("level.accuratepercent.bugfix", true))
                return utils::getActualProgress(this);
            return PlayLayer::getCurrentPercent();
        }

        void updateProgressbar() {
            PlayLayer::updateProgressbar();
            if (!config::get<bool>("level.accuratepercentage", false)) return;
            if (m_percentageLabel == nullptr) return;

            if (m_level->isPlatformer()) {
                if (!config::get<bool>("level.accuratepercent.show_minutes", true)) return;
                auto time = utils::formatTime(m_gameState.m_levelTime);
                m_percentageLabel->setString(time.c_str());
            } else if (config::get<bool>("level.accuratepercent.normal_mode", true)) {
                float percent = customGetProgress();
                auto numDigits = config::get<int>("level.accuratepercent.amount", 4);
                m_percentageLabel->setString(fmt::format("{:.{}f}%", percent, numDigits).c_str());

                // If bugfix is active, also fix the progress bar
                if (!config::get<bool>("level.accuratepercent.bugfix", true)) return;
                m_progressFill->setTextureRect({
                    0, 0,
                    (m_progressBar->getTextureRect().getMaxX() - 5) * percent / 100.f,
                    m_progressBar->getTextureRect().getMaxY() / 2
                });
            }
        }
    };
}
