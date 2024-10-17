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
                ->setDescription("Makes list backgrounds transparent.");
        }

        [[nodiscard]] const char* getId() const override { return "Transparent Lists"; }
    };

    REGISTER_HACK(TransparentLists)

    class $modify(TransparentListsCCLCHook, cocos2d::CCLayerColor) {
        ADD_HOOKS_DELEGATE("global.transparentlists")

        bool initWithColor(const cocos2d::ccColor4B& yk, float f1, float f2) {
            bool ret = CCLayerColor::initWithColor(yk, f1, f2);

            if (yk == cocos2d::ccColor4B{161, 88, 44, 255} || yk == cocos2d::ccColor4B{194, 114, 62, 255}) // the 2 colors GD uses for list cells
                this->setVisible(false);

            return ret;
        }

        bool initWithColor(const cocos2d::ccColor4B& yk) {
            bool ret = CCLayerColor::initWithColor(yk);

            if (yk == cocos2d::ccColor4B{161, 88, 44, 255} || yk == cocos2d::ccColor4B{194, 114, 62, 255}) { // the 2 colors GD uses for list cells
                this->setOpacity(0);
                this->setVisible(false);
            }

            return ret;
        }
    };


}
