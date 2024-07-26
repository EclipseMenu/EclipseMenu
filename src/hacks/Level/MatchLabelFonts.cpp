#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class MatchLabelFonts : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Match Percent/Time Fonts", "level.matchlabelfonts")
                ->setDescription("Matches the level percentage/level time UI element's font with the font used in a level. (Created by RayDeeUx)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Match Percent/Time Fonts"; }
    };

    REGISTER_HACK(MatchLabelFonts)

    class $modify(MyPlayLayer, PlayLayer) {
        void onEnterTransitionDidFinish() {
            PlayLayer::onEnterTransitionDidFinish();

            if (m_attemptLabel && m_percentageLabel && config::get<bool>("level.matchlabelfonts", false))
                m_percentageLabel->setFntFile(m_attemptLabel->getFntFile());
        }
    };

}
