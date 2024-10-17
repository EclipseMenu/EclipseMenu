#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCTextInputNode.hpp>
#include <utility>

namespace eclipse::hacks::Bypass {

    class CharFilter : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Character Filter Bypass", "bypass.charfilter")
                ->handleKeybinds()
                ->setDescription("Allows you to bypass the character filter on any text field.");
        }

        [[nodiscard]] const char* getId() const override { return "Character Filter Bypass"; }
    };

    REGISTER_HACK(CharFilter)

    class $modify(CharacterFilterCCTINHook, CCTextInputNode) {
        ADD_HOOKS_DELEGATE("bypass.charfilter")

        void updateLabel(gd::string str) {
            this->setAllowedChars(
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789!@#$%^&*()-=_+"
                "`~[]{}/?.>,<\\|;:'\""
                " "
            );
 
            CCTextInputNode::updateLabel(std::move(str));
        }
    };

}
