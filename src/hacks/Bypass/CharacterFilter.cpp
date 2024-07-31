#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCTextInputNode.hpp>

namespace eclipse::hacks::Bypass {

    class CharFilter : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Character Filter Bypass", "bypass.charfilter")
                ->handleKeybinds()
                ->setDescription("Allows you to bypass the character filter on any text field");
        }

        [[nodiscard]] const char* getId() const override { return "Character Filter Bypass"; }
    };

    REGISTER_HACK(CharFilter)

    class $modify(CharacterFilterCCTINHook, CCTextInputNode) {
        void updateLabel(gd::string str) {
            // im just gonna hope this is all of it
            if (config::get<bool>("bypass.charfilter", false))
                setAllowedChars("`1234567890-=qwertyuiop[]\\asdfghjkl;'zxcvbnm,.+_|{}:?/!@#$%^&*()");
 
            CCTextInputNode::updateLabel(str);
        }
    };

}
