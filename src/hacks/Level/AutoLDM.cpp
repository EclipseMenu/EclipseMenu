#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoLDM : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Auto LDM", "level.autoldm")
                ->handleKeybinds()
                ->setDescription("Automatically enables LDM upon entering a level");
        }

        [[nodiscard]] const char* getId() const override { return "Auto LDM"; }
    };

    REGISTER_HACK(AutoLDM)

    class $modify(AutoLDMELLHook, EditLevelLayer) {
        ADD_HOOKS_DELEGATE("level.autoldm")

        bool init(GJGameLevel* level) {
            level->m_lowDetailModeToggled = true;
            return EditLevelLayer::init(level);
        }
    };

    class $modify(AutoLDMLILHook, LevelInfoLayer) {
        ADD_HOOKS_DELEGATE("level.autoldm")

        bool init(GJGameLevel* level, bool challenge) {
            level->m_lowDetailModeToggled = true;
            return LevelInfoLayer::init(level, challenge);
        }
    };
}
