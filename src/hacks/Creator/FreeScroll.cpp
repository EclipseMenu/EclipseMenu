#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditorUI.hpp>

namespace eclipse::hacks::Creator {

    class FreeScroll : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Free Scroll", "creator.freescroll")
                ->handleKeybinds()
                ->setDescription("Removes camera constraints in the level editor");
        }

        [[nodiscard]] const char* getId() const override { return "Free Scroll"; }
    };

    REGISTER_HACK(FreeScroll)

    class $modify(FreeScrollEUIHook, EditorUI) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("EditorUI::constrainGameLayerPosition");
        }

        void constrainGameLayerPosition(float width, float height) {
            if (!config::get<bool>("creator.freescroll", false))
                return EditorUI::constrainGameLayerPosition(width, height);

            // decompiled function:
            // void EditorUI::constrainGameLayerPosition(float width, float height) {
            //     auto* objLayer = m_editorLayer->m_objectLayer;
            //     auto pos = objLayer->getPosition();
            //     objLayer->setPosition({0, 0});
            //     auto* director = cocos2d::CCDirector::sharedDirector();
            //     auto screenLeft = director->getScreenLeft();
            //     auto screenTop = director->getScreenTop();
            //     auto screenRight = director->getScreenRight();
            //
            //     cocos2d::CCPoint topRight = { screenRight, screenTop };
            //     cocos2d::CCPoint bottomLeft = { screenLeft, m_unk23c };
            //
            //     auto maxBlocksHeight = m_editorLayer->m_levelSettings->m_dynamicLevelHeight ? 1000 : 80;
            //     cocos2d::CCPoint worldBottomLeft = objLayer->convertToWorldSpace({ width * 30.0 - 30.0, height * 30.0 + 90.0 });
            //     cocos2d::CCPoint worldTopRight = objLayer->convertToWorldSpace({ 240030.0, (maxBlocksHeight * 30.0) + 90.0 + 30.0 });
            //
            //     auto x = std::clamp(pos.x, bottomLeft.x - worldBottomLeft.x, topRight.x - worldTopRight.x);
            //     auto y = std::clamp(pos.y, bottomLeft.y - worldBottomLeft.y, topRight.y - worldTopRight.y);
            //
            //     objLayer->setPosition({x, y});
            // }
        }
    };

}
