#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class MatchLabelFonts : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.matchlabelfonts")->setDescription()->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Match Percent/Time Fonts"; }
    };

    REGISTER_HACK(MatchLabelFonts)

    static const std::string s_bigFontName = "bigFont.fnt";

    class $modify(MatchLabelFontsPLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.matchlabelfonts")

        bool shouldMatchLabelFonts() const {
            return m_attemptLabel && m_percentageLabel &&
                   m_percentageLabel->getFntFile() == s_bigFontName &&
                   m_attemptLabel->getFntFile() != s_bigFontName;
        }

        void updateProgressbar() {
            PlayLayer::updateProgressbar();

            if (shouldMatchLabelFonts())
                m_percentageLabel->setFntFile(m_attemptLabel->getFntFile());
        }

        void levelComplete() {
            PlayLayer::levelComplete();

            if (shouldMatchLabelFonts())
                m_percentageLabel->setFntFile(m_attemptLabel->getFntFile());
        }
    };
}
