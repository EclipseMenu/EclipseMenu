#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EditorUI.hpp>

namespace eclipse::hacks::Creator {
    class $hack(FreeScroll) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.freescroll")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Free Scroll"; }
    };

    REGISTER_HACK(FreeScroll)

    class $modify(FreeScrollEUIHook, EditorUI) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.freescroll")

        void constrainGameLayerPosition(float width, float height) {
            // decompiled function:
            // auto* objLayer = m_editorLayer->m_objectLayer;
            // auto pos = objLayer->getPosition();
            // objLayer->setPosition({0, 0});
            // auto* director = utils::get<cocos2d::CCDirector>();
            // auto screenLeft = director->getScreenLeft();
            // auto screenTop = director->getScreenTop();
            // auto screenRight = director->getScreenRight();
            //
            // cocos2d::CCPoint topRight = { screenRight, screenTop };
            // cocos2d::CCPoint bottomLeft = { screenLeft, m_unk23c };
            //
            // auto maxBlocksHeight = m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 1000 : 80;
            // cocos2d::CCPoint worldBottomLeft = objLayer->convertToWorldSpace({ width * 30.0 - 30.0, height * 30.0 + 90.0 });
            // cocos2d::CCPoint worldTopRight = objLayer->convertToWorldSpace({ 240030.0, (maxBlocksHeight * 30.0) + 90.0 + 30.0 });
            //
            // auto x = std::clamp(pos.x, bottomLeft.x - worldBottomLeft.x, topRight.x - worldTopRight.x);
            // auto y = std::clamp(pos.y, bottomLeft.y - worldBottomLeft.y, topRight.y - worldTopRight.y);
            //
            // objLayer->setPosition({x, y});
        }
    };
}
