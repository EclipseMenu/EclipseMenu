#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditorPauseLayer.hpp>

namespace eclipse::hacks::Creator {
    class ResetPercentOnSave : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");
            tab->addToggle("Reset Percent on Save", "creator.resetpercentonsave")
                ->handleKeybinds()
                ->setDescription("Resets a level's percent to 0% when saved/unverified.");
        }

        [[nodiscard]] const char* getId() const override { return "Reset Percent on Save"; }
    };

    REGISTER_HACK(ResetPercentOnSave)

    class $modify(ResetPercentEPLHook, EditorPauseLayer) {
        ADD_HOOKS_DELEGATE("creator.resetpercentonsave")
        void saveLevel() {
            m_editorLayer->m_level->m_normalPercent = 0;
            EditorPauseLayer::saveLevel();
        }
    };
}
