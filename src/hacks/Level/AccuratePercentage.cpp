#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Global {
    class $hack(AccuratePercentage) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.accuratepercentage.amount", 4);
            config::setIfEmpty("level.accuratepercentage.normal_mode", true);
            config::setIfEmpty("level.accuratepercentage.bugfix", true);
            config::setIfEmpty("level.accuratepercentage.show_minutes", true);

            tab->addToggle("level.accuratepercentage")->setDescription()->handleKeybinds()
               ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                   options->addToggle("level.accuratepercentage.normal_mode");
                   options->addInputInt("level.accuratepercentage.amount", 0, 15);
                   options->addToggle("level.accuratepercentage.bugfix");
                   options->addToggle("level.accuratepercentage.show_minutes");
               });
        }

        [[nodiscard]] const char* getId() const override { return "Accurate Percentage"; }
    };

    REGISTER_HACK(AccuratePercentage)

    class $modify(AccuratePercentagePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.accuratepercentage")

        float customGetProgress() {
            if (config::get<"level.accuratepercentage.bugfix", bool>(true))
                return utils::getActualProgress(this);
            return this->getCurrentPercent();
        }

        void updateProgressbar() {
            PlayLayer::updateProgressbar();
            if (m_percentageLabel == nullptr) return;

            if (m_level->isPlatformer()) {
                if (!config::get<"level.accuratepercentage.show_minutes", bool>(true)) return;
                auto time = utils::formatTime(m_timePlayed);
                m_percentageLabel->setString(time.c_str());
            } else if (config::get<"level.accuratepercentage.normal_mode", bool>(true)) {
                float percent = customGetProgress();
                auto numDigits = config::get<int>("level.accuratepercentage.amount", 4);
                if (numDigits > 1) {
                    m_percentageLabel->setString(fmt::format("{:.{}f}%", percent, numDigits).c_str());
                } else {
                    m_percentageLabel->setString(fmt::format("{}%", static_cast<int>(std::floor(percent))).c_str());
                }

                // If bugfix is active, also fix the progress bar
                if (!config::get<"level.accuratepercentage.bugfix", bool>(true)) return;
                m_progressFill->setTextureRect({
                    0, 0,
                    (m_progressBar->getTextureRect().getMaxX() - 5) * percent / 100.f,
                    m_progressBar->getTextureRect().getMaxY() / 2
                });
            }
        }
    };
}
