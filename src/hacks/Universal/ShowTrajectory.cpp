#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CCLayerColor.hpp>

namespace eclipse::hacks::Universal {
    class TransparentLists : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Universal");
            tab->addToggle("Transparent Lists", "universal.transparentlists")->setDescription("Makes list backgrounds transparent");
        }

        void update() override {}
        [[nodiscard]] const char* getId() const override { return "Transparent Lists"; }
    };

    REGISTER_HACK(TransparentLists)
    using namespace geode::prelude;
    class $modify(CCLayerColor) {
	bool initWithColor(cocos2d::_ccColor4B const& yk, float f1, float f2) {
		if (config::get<bool>("universal.transparentlists", false)) {
			return CCLayerColor::initWithColor(ccc4(0, 0, 0, 0), 0, 0);
		} else {
			return CCLayerColor::initWithColor(yk, f1, f2);
		} 
	}
};


}
