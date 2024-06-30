#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/GJGameLevel.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/EndLevelLayer.hpp>
bool nodeidsloaded = false;
bool Sm_Mod = false;
using namespace geode::prelude;

CCNode* getChildBySpriteFrameName_1(CCNode* parent, const char* name) {
    auto cache = CCSpriteFrameCache::sharedSpriteFrameCache()->spriteFrameByName(name);
    if (!cache) return nullptr;

    auto* texture = cache->getTexture();
    auto rect = cache->getRect();

    for (int i = 0; i < parent->getChildrenCount(); ++i) {
        auto* child = parent->getChildren()->objectAtIndex(i);
        if (auto* spr = typeinfo_cast<CCSprite*>(child)) {
            if (spr->getTexture() == texture && spr->getTextureRect() == rect) {
                return spr;
            }
        } else if (auto* btn = typeinfo_cast<CCMenuItemSprite*>(child)) {
            auto* img = btn->getNormalImage();
            if (auto* spr = typeinfo_cast<CCSprite*>(img)) {
                if (spr->getTexture() == texture && spr->getTextureRect() == rect) {
                    return btn;
                }
            }
        }
    }
    return nullptr;
}


namespace eclipse::hacks::Level {

    class safemode : public hack::Hack {
        void init() override {
            config::setIfEmpty("level.safemode.legacy", false);
            auto tab = gui::MenuTab::find("Level");
            auto tav = tab->addToggle("Safe Mode", "level.safemode");
            tav->setDescription("Allows you to not save progress when enabled!");
            // someone add options for legacy (aka kick on loading endscreen)
        }

        [[nodiscard]] const char* getId() const override { return "Safe Mode"; }
    };

    REGISTER_HACK(safemode)


    class autosafemode : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            auto tav = tab->addToggle("Auto Safe Mode", "level.autosafemode");
            tav->setDescription("Auto enables safe mode if cheats are on!");
            // someone add options for legacy (aka kick on loading endscreen)
        }

        [[nodiscard]] const char* getId() const override { return "Auto Safe Mode"; }
    };

    REGISTER_HACK(autosafemode)


    bool Cheats() {
        bool cheats = false;
        for (eclipse::hack::Hack* hack : eclipse::hack::Hack::getHacks()) {
            if (hack->isCheating()) {
                cheats = true;
                break;
            }
        }
        return cheats;
    }
    bool savedata() {
        if config::get<bool>("level.autosafemode") {
           // safemode doesn't override auto
            if (Cheats()) {
                return false;
            } else {
                return true;
            }
        }
         if (!config::get<bool>("level.safemode") ) {
            return true;
         } else {
            return false;
         }
    }
   class $modify(GJGameLevel) {
    void savePercentage(int p0, bool p1, int p2, int p3, bool p4) {
        if (savedata()) {
            GJGameLevel::savePercentage(p0, p1, p2, p3, p4);
        }
    }
};

class $modify(PlayLayer) {
    void showNewBest(bool po, int p1, int p2, bool p3, bool p4, bool p5) {
           if (savedata()) {
            PlayLayer::showNewBest(po, p1, p2 , p3 , p4 , p5);
          }
    };
    void levelComplete() {
        if (savedata()) {
            if (config::get<bool>("level.safemode.legacy", true)) {
            PlayLayer::get()->onQuit();
            return;
         };

            PlayLayer::get()->m_isTestMode = true;  // gd doesn't save in test mode
            Sm_Mod = true;  
        } else {
            Sm_Mod = false;
        }

        PlayLayer::levelComplete();
    };
};
class $modify(endLayer,EndLevelLayer){
static void onModify(auto & self)
    {
         (void) self.setHookPriority("EndLevelLayer::showLayer", -100);
    };

    void showLayer(bool p0) {
 	    EndLevelLayer::showLayer(p0);
        
        if (!Sm_Mod) {
            return;
        }
        if (Loader::get()->isModLoaded("geode.node-ids")) {
            nodeidsloaded = true; // node ids just helps
        } else {
            nodeidsloaded = false;
        }
        cocos2d::CCNode* Layer = nullptr;
        if (nodeidsloaded) {
            Layer =  this->getChildByID("main-layer"); 
        } else {
           Layer = geode::cocos::getChildOfType<cocos2d::CCLayer>(this, 0);
        }
         cocos2d::CCNode* endcompletetext = nullptr;
         if (nodeidsloaded) {
            endcompletetext = Layer->getChildByID("practice-complete-text");
            if (!endcompletetext) { endcompletetext =  Layer->getChildByID("level-complete-text"); };
        } else {
            if(auto LevelComplete = getChildBySpriteFrameName_1(Layer, "GJ_levelComplete_001.png")) {
        		endcompletetext = LevelComplete;
    		}
	        else if (auto practiceTxt = getChildBySpriteFrameName_1(Layer, "GJ_practiceComplete_001.png")) {
       		 endcompletetext = practiceTxt;
  	        }     
          
        };
        

        if (endcompletetext) {
			endcompletetext->setVisible(false);
            CCSprite* SafeMode = nullptr;
            if (Cheats()) {
                SafeMode = CCSprite::createWithSpriteFrameName("SafeModeCheats.png"_spr);
            }
            else {
                SafeMode = CCSprite::createWithSpriteFrameName("SafeModeNoCheats.png"_spr);
            };

            SafeMode->setID("Safe-complete-text"_spr);
            Layer->addChild(SafeMode);
            SafeMode->setPosition(endcompletetext->getPosition());
		}
        CCNode* CompleteText = nullptr;
         if (nodeidsloaded) {
            CompleteText = Layer->getChildByID("complete-message");
         } else {
             CompleteText = geode::cocos::getChildOfType<TextArea>(Layer, 0);
         }
        if (CompleteText) {
            CompleteText->setVisible(false);
           CCLabelBMFont* weed = CCLabelBMFont::create("SAFE MODE ENABLED!", "bigFont.fnt");
			weed->setAnchorPoint(ccp(0.480f,1.0f));
			weed->setPosition(CompleteText->getPosition());
			weed->setScale(0.5f);	
			weed->setID("complete-message"_spr);
			Layer->addChild(weed);
        }
        } 
        
};


}