#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCLayerColor.hpp>

namespace eclipse::hacks::Global {
    class TransparentLists : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            tab->addToggle("Transparent Lists", "global.transparentlists")
                ->handleKeybinds()
                ->setDescription("Makes list backgrounds transparent");
        }

        [[nodiscard]] const char* getId() const override { return "Transparent Lists"; }
    };

    REGISTER_HACK(TransparentLists)

    class $modify(TransparentListsCCLCHook, cocos2d::CCLayerColor) {
        ADD_HOOKS_DELEGATE("global.transparentlists")

        bool initWithColor(const cocos2d::ccColor4B& yk, float f1, float f2) {
            return CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0), 0, 0);
        }
    };


}
