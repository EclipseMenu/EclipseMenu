#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCLayerColor.hpp>

namespace eclipse::hacks::Global {
    class TransparentLists : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");
            tab->addToggle("Transparent Lists", "global.transparentlists")->setDescription("Makes list backgrounds transparent");
        }

        [[nodiscard]] const char* getId() const override { return "Transparent Lists"; }
    };

    REGISTER_HACK(TransparentLists)

    class $modify(cocos2d::CCLayerColor) {
	    bool initWithColor(cocos2d::_ccColor4B const& yk, float f1, float f2) {
            if (config::get<bool>("global.transparentlists", false)) {
                return cocos2d::CCLayerColor::initWithColor(cocos2d::ccc4(0, 0, 0, 0), 0, 0);
            } else {
                return cocos2d::CCLayerColor::initWithColor(yk, f1, f2);
            }
        }
    };


}
