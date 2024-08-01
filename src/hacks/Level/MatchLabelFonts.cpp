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

    static const std::string s_bigFontName = "bigFont.fnt";

    class $modify(MatchLabelFontsPLHook, PlayLayer) {

        bool shouldMatchLabelFonts() {
            return m_attemptLabel && m_percentageLabel &&
                m_percentageLabel->getFntFile() == s_bigFontName &&
                m_attemptLabel->getFntFile() != s_bigFontName;
        }

        void updateProgressbar() {
            PlayLayer::updateProgressbar();

            if (shouldMatchLabelFonts() && config::get<bool>("level.matchlabelfonts", false))
                m_percentageLabel->setFntFile(m_attemptLabel->getFntFile());
        }

        void levelComplete() {
            PlayLayer::levelComplete();

            if (shouldMatchLabelFonts() && config::get<bool>("level.matchlabelfonts", false))
                m_percentageLabel->setFntFile(m_attemptLabel->getFntFile());
        }
    };

}
