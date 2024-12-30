#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Bypass {
    class CheckpointLimit : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.checkpointlimit")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Checkpoint Limit"; }
    };

    REGISTER_HACK(CheckpointLimit)

    class $modify(CheckpointLimitPLHook, PlayLayer) {
        ALL_DELEGATES_AND_SAFE_PRIO("bypass.checkpointlimit")

        void storeCheckpoint(CheckpointObject* checkpointObject) {
            // Reimplemented without the checkpoint limit
            m_checkpointArray->addObject(checkpointObject);
            PlayLayer::addToSection(checkpointObject->m_physicalCheckpointObject);
        }
    };
}
