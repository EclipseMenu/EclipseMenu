#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoLDM : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Auto LDM", "level.autoldm");
        }

        [[nodiscard]] const char* getId() const override { return "Auto LDM"; }
    };

    REGISTER_HACK(AutoLDM)

    class $modify(EditLevelLayer) {
        static EditLevelLayer* create(GJGameLevel* level) {
            if (config::get<bool>("level.autoldm", false)) level->m_lowDetailModeToggled = true;
            return EditLevelLayer::create(level);
        }
    };

    class $modify(LevelInfoLayer) {
        static LevelInfoLayer* create(GJGameLevel* level, bool p1) {
            if (config::get<bool>("level.autoldm", false)) level->m_lowDetailModeToggled = true;
            return LevelInfoLayer::create(level, p1);
        }
    };
}