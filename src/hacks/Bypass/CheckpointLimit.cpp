#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Bypass {

    class CheckpointLimit : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Checkpoint Limit", "bypass.checkpointlimit")
                ->handleKeybinds()
                ->setDescription("Allows you to place more than 50 checkpoints in practice mode.");
        }

        [[nodiscard]] const char* getId() const override { return "Checkpoint Limit"; }
    };

    REGISTER_HACK(CheckpointLimit)

    class $modify(PlayLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("PlayLayer::storeCheckpoint");
        }

        void storeCheckpoint(CheckpointObject* checkpointObject) {
            if (!config::get<bool>("bypass.checkpointlimit", false))
                return PlayLayer::storeCheckpoint(checkpointObject);

            // Reimplemented without the checkpoint limit
            m_checkpointArray->addObject(checkpointObject);
            PlayLayer::addToSection(checkpointObject->m_physicalCheckpointObject);
        }
    };

}
