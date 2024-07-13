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
                ->setDescription("Allows for more decimals in a level percentage.")
                ->handleKeybinds()
                ->addOptions([] (gui::MenuTab* options) {
                    options->addToggle("Normal Mode", "level.accuratepercent.normal_mode");
                    options->addInputInt("Decimal Places", "level.accuratepercent.amount", 0, 15);
                    options->addToggle("Fix 0% bug", "level.accuratepercent.bugfix");
                    options->addToggle("Show Minutes", "level.accuratepercent.show_minutes");
                });
        }

        [[nodiscard]] const char* getId() const override { return "Accurate Percentage"; }
    };

    REGISTER_HACK(AccuratePercentage)

    class $modify(PlayLayer) {
        float customGetProgress() {
            if (config::get<bool>("level.accuratepercent.bugfix", true)) {
                float percent;
                if (m_level->m_timestamp > 0) {
                    percent = static_cast<float>(m_gameState.m_levelTime * 240.f) / m_level->m_timestamp * 100.f;
                } else {
                    percent = reinterpret_cast<cocos2d::CCNode*>(m_player1)->getPositionX() / m_levelLength * 100.f;
                }
                return std::clamp(percent, 0.f, 100.f);
            }
            return PlayLayer::getCurrentPercent();
        }

        void updateProgressbar() {
            PlayLayer::updateProgressbar();
            if (!config::get<bool>("level.accuratepercentage", false)) return;
            if (m_percentageLabel == nullptr) return;

            if (m_level->isPlatformer()) {
                if (!config::get<bool>("level.accuratepercent.show_minutes", true)) return;
                auto time = m_gameState.m_levelTime;
                auto minutes = static_cast<int>(time / 60);
                auto seconds = static_cast<int>(time) % 60;
                auto millis = static_cast<int>(time * 1000) % 1000;

                std::string timeStr;
                if (minutes > 0) {
                    timeStr = fmt::format("{}:{:02d}.{:03d}", minutes, seconds, millis);
                } else {
                    timeStr = fmt::format("{}.{:03d}", seconds, millis);
                }
                m_percentageLabel->setString(timeStr.c_str());
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
