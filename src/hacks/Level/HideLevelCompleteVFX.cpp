#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class HideLevelCompleteVFX : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");
            tab->addToggle("Hide Level Complete VFX", "level.hidelevelcomplete")
                ->setDescription("Hides the explosion and fireworks seen when completing a level. (Does not hide particles.)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Hide Level Complete VFX"; }
    };

    REGISTER_HACK(HideLevelCompleteVFX)

    class $modify(EclipsePlayLayer, PlayLayer) {
        struct Fields {
            bool isLevelComplete = false;
        };
        void eclipseHideVFX() {
            if (!config::get<bool>("level.hidelevelcomplete") || !m_fields->isLevelComplete) { return; }
            for (unsigned int i = getChildrenCount(); i-- > 0; ) {
                auto theObject = getChildren()->objectAtIndex(i);
                if (const auto ccCircleWave = geode::cast::typeinfo_cast<CCCircleWave*>(theObject)) {
                    ccCircleWave->setVisible(false);
                }
            }
            if (const auto mainNode = getChildByIDRecursive("main-node")) {
                for (CCNode* mainNodeChild : geode::cocos::CCArrayExt<CCNode*>(mainNode->getChildren())) {
                    if (const auto whereEverythingIs = geode::cast::typeinfo_cast<CCLayer*>(mainNodeChild)) {
                        for (CCNode* childTwo : geode::cocos::CCArrayExt<CCNode*>(whereEverythingIs->getChildren())) {
                            if (const auto ccCircleWave = geode::cast::typeinfo_cast<CCCircleWave*>(childTwo)) {
                                ccCircleWave->setVisible(false);
                            } else if (const auto ccLightFlash = geode::cast::typeinfo_cast<CCLightFlash*>(childTwo)) {
                                ccLightFlash->setVisible(false);
                            }
                        }
                    }
                }
            }
        }
        void onQuit() {
            m_fields->isLevelComplete = false;
            PlayLayer::onQuit();
        }
        void postUpdate(float p0) {
            PlayLayer::postUpdate(p0);
            if (!config::get<bool>("level.hidelevelcomplete") || !m_fields->isLevelComplete) { return; }
            EclipsePlayLayer::eclipseHideVFX();
        }
        void levelComplete() {
            PlayLayer::levelComplete();
            if (!config::get<bool>("level.hidelevelcomplete")) { return; }
            m_fields->isLevelComplete = true;
            EclipsePlayLayer::eclipseHideVFX();
        }
    };
}