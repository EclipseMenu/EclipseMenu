#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class FreezeAttempts : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Freeze Attempts", "level.freezeatts")->setDescription("Freezes the attempt counter, preventing any new attempts from being saved");
        }

        [[nodiscard]] const char* getId() const override { return "Freeze Attempts"; }
    };

    REGISTER_HACK(FreezeAttempts)
    
    class $modify(PlayLayer) {
        void updateAttempts() {
            if (!config::get<bool>("level.freezeatts", false)) PlayLayer::updateAttempts();
        }
    };
}
