#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EditLevelLayer.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(VerifyHack) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("bypass.verifyhack")->handleKeybinds()->setDescription();
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
