#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Global {
    class AccuratePercentage : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            config::setIfEmpty("level.accuratepercent.amount", 4);
            tab->addToggle("Accurate Percentage", "level.accuratepercentage")->setDescription("Allows for more decimals in a level percentage.")
            ->addOptions([] (gui::MenuTab* options) {
                options->addInputInt("Amount", "level.accuratepercent.amount", 0, 15);
            });
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Accurate Percentage"; }
    };

    REGISTER_HACK(AccuratePercentage)

    class $modify(PlayLayer) {
	    void updateProgressbar() {
            PlayLayer::updateProgressbar();
            if (!config::get<bool>("level.accuratepercentage", false)) {
                if (m_percentageLabel == nullptr) return;
                std::stringstream percentStream;
                int numDigits = config::get<int>("level.accuratepercent.amount", 4);
                percentStream << std::fixed << std::setprecision(numDigits) << PlayLayer::getCurrentPercent() << "%";
                std::string percentStr = percentStream.str();
                m_percentageLabel->setString(percentStr.c_str());
            }
        }
    };



}
