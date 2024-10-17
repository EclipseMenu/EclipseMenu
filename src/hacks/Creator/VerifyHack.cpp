#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditLevelLayer.hpp>

namespace eclipse::hacks::Bypass {

    class VerifyHack : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Verify Bypass", "bypass.verifyhack")
                ->handleKeybinds()
                ->setDescription("Automatically verifies levels for you.");
        }

        [[nodiscard]] const char* getId() const override { return "Verify Bypass"; }
    };

    REGISTER_HACK(VerifyHack)

    class $modify(VerifyHackELLHook, EditLevelLayer) {
        ADD_HOOKS_DELEGATE("bypass.verifyhack")

        bool init(GJGameLevel* gl) {
            gl->m_isVerified = true;
            return EditLevelLayer::init(gl);
        }
    };
};
