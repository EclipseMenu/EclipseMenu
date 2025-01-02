#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/CCTextInputNode.hpp>

namespace eclipse::hacks::Bypass {
    class CharLimit : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.charlimit")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Character Limit Bypass"; }
    };

    REGISTER_HACK(CharLimit)

    class $modify(CharacterLimitCCTINHook, CCTextInputNode) {
        ADD_HOOKS_DELEGATE("bypass.charlimit")

        void updateLabel(gd::string str) {
            this->setMaxLabelLength(99999);
            CCTextInputNode::updateLabel(str);
        }
    };
}
