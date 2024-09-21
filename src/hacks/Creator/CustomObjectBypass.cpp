#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditorUI.hpp>

namespace eclipse::hacks::Creator {

    class CustomObjectBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Creator");

            tab->addToggle("Custom Object Bypass", "creator.customobjectbypass")
                ->handleKeybinds()
                ->setDescription("Allows you to go beyond the 1000 object limit for custom objects.");
        }

        [[nodiscard]] const char* getId() const override { return "Custom Object Bypass"; }
    };

    REGISTER_HACK(CustomObjectBypass)

    class $modify(CustomObjectBypassHook, EditorUI) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("EditorUI::onNewCustomItem");
        }
        void onNewCustomItem(CCObject* sender) {
            if (!config::get<bool>("creator.customobjectbypass", false))
                return EditorUI::onNewCustomItem(sender);
            if (auto gameManager = GameManager::sharedState()) {
                cocos2d::CCArray* newSelectedObjs;
                if (m_selectedObjects->count() == 0) {
                    newSelectedObjs = cocos2d::CCArray::create();
                    newSelectedObjs->addObject(m_selectedObject);
                } else {
                    newSelectedObjs = this->m_selectedObjects;
                }
                gameManager->addNewCustomObject(copyObjects(newSelectedObjs, false, false));
                m_selectedObjectIndex = 0;
                reloadCustomItems();
            }
        }
    };
}
