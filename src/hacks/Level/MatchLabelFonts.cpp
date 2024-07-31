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

    class $modify(MatchLabelFontsPLHook, PlayLayer) {
        struct Fields {
            const std::string bigFontName = "bigFont.fnt";
        };

        void matchLabelFonts() {
            m_percentageLabel->setFntFile(m_attemptLabel->getFntFile());
        }

        bool labelsExist() {
            return m_attemptLabel && m_percentageLabel;
        }

        bool percentLabelBigFnt() {
            return m_percentageLabel->getFntFile() == m_fields->bigFontName;
        }

        bool attemptLabelNotBigFnt() {
            return m_attemptLabel->getFntFile() != m_fields->bigFontName;
        }

        bool shouldMatchLabelFonts() {
            return labelsExist() && percentLabelBigFnt() && attemptLabelNotBigFnt();
        }

        void updateProgressbar() {
            PlayLayer::updateProgressbar();

            if (shouldMatchLabelFonts() && config::get<bool>("level.matchlabelfonts", false))
                matchLabelFonts();
        }

        void levelComplete() {
            PlayLayer::levelComplete();

            if (shouldMatchLabelFonts() && config::get<bool>("level.matchlabelfonts", false))
                matchLabelFonts();
        }
    };

}
