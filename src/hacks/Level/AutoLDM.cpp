#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoLDM : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Auto LDM", "level.autoldm")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Auto LDM"; }
    };

    REGISTER_HACK(AutoLDM)

    class $modify(AutoLDMELLHook, EditLevelLayer) {
        bool init(GJGameLevel* level) {
            if (config::get<bool>("level.autoldm", false))
                level->m_lowDetailModeToggled = true;

            return EditLevelLayer::init(level);
        }
    };

    class $modify(AutoLDMLILHook, LevelInfoLayer) {
        bool init(GJGameLevel* level, bool challenge) {
            if (config::get<bool>("level.autoldm", false))
                level->m_lowDetailModeToggled = true;

            return LevelInfoLayer::init(level, challenge);
        }
    };
}