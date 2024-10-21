#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCTextInputNode.hpp>

namespace eclipse::hacks::Bypass {

    class CharLimit : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Character Limit Bypass", "bypass.charlimit")
                ->handleKeybinds()
                ->setDescription("Allows you to bypass the character limit of any text field.");
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
