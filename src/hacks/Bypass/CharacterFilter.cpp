#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCTextInputNode.hpp>
#include <utility>

namespace eclipse::hacks::Bypass {

    class CharFilter : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");

            tab->addToggle("bypass.charfilter")
                ->handleKeybinds()
                ->setDescription();
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
