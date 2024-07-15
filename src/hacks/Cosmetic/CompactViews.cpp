#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/CustomListView.hpp>
#include <Geode/modify/LevelCell.hpp>

namespace eclipse::hacks::Cosmetic {

    class CompactViews : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Cosmetic");
            tab->addToggle("Compact Editor Levels", "cosmetic.compacteditorlevels")
                ->handleKeybinds()
                ->setDescription("Enables the compact view when browsing custom editor levels. Adapted from code by Cvolton.");
            tab->addToggle("Compact Profile Comments", "cosmetic.compactprofilecomments")
                ->handleKeybinds()
                ->setDescription("Enables the compact view when viewing profile comments. Adapted from code by Cvolton.");
        }

        [[nodiscard]] const char* getId() const override { return "Compact Views"; }
    };

    REGISTER_HACK(CompactViews);

    class $modify(EclipseCustomListView, CustomListView) {
    	/*
    	original code by cvolton, re-used for "my levels" list
    	and now reused for profile comments yayy
    	proof of consent of code reuse: https://discord.com/channels/911701438269386882/911702535373475870/1220117410988953762
    	and now it finds a new home in eclipsemenu
    	*/
        static CustomListView* create(cocos2d::CCArray* a, TableViewCellDelegate* b, float c, float d, int e, BoomListType f, float g) {
            if (f == BoomListType::Level2 && config::get<bool>("cosmetic.compacteditorlevels", true)) {
                f = BoomListType::Level4; // Level4 = compact level view
            } else if (f == BoomListType::Comment4 && config::get<bool>("cosmetic.compactprofilecomments", true)) {
                f = BoomListType::Comment2; // Comment2 = compact comment view
            }
            return CustomListView::create(a, b, c, d, e, f, g);
        }
    };

	class $modify(EclipseLevelCell, LevelCell) {
		void onClick(CCObject* sender) {
			// hooking this function is necessary in order for the "view" button to work while compact mode is active in "my levels"
			if (this->m_level->m_levelType == GJLevelType::Editor && config::get<bool>("cosmetic.compacteditorlevels", true)) {
				const auto scene = cocos2d::CCScene::create();
				scene->addChild(EditLevelLayer::create(m_level));
				cocos2d::CCDirector::sharedDirector()->replaceScene(cocos2d::CCTransitionFade::create(0.5f, scene));
			} else { LevelCell::onClick(sender); }
		}
		void loadLocalLevelCell() {
			LevelCell::loadLocalLevelCell();
			if (config::get<bool>("cosmetic.compacteditorlevels", false)) { return; }
			if (const auto localLevelname = geode::cast::typeinfo_cast<cocos2d::CCLabelBMFont*>(getChildByIDRecursive("level-name"))) {
				localLevelname->limitLabelWidth(200.f, .6f, .01f);
			}
			if (const auto mainLayer = geode::cast::typeinfo_cast<CCLayer*>(getChildByIDRecursive("main-layer"))) {
				mainLayer->setPositionY(-3.5f);
			}
		}
	};
}