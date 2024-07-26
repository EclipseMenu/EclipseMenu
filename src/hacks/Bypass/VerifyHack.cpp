#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>

namespace eclipse::hacks::Bypass {

    class VerifyHack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Verify Bypass", "bypass.verifyhack")
                ->handleKeybinds()
                ->setDescription("Automatically verifies levels for you");
        }

        [[nodiscard]] const char* getId() const override { return "Verify Bypass"; }
    };

    REGISTER_HACK(VerifyHack)

    class $modify(EditLevelLayer) {
        bool init(GJGameLevel* gl) {
            if (config::get<bool>("bypass.verifyhack", false)) gl->m_isVerified = true;
            return EditLevelLayer::init(gl);
        }
    };
};
