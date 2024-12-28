#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/EditorUI.hpp>

namespace eclipse::hacks::Creator {

    class CustomObjectBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");

            tab->addToggle("creator.customobjectbypass")
                ->handleKeybinds()
                ->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Custom Object Bypass"; }
    };

    REGISTER_HACK(CustomObjectBypass)

    class $modify(CustomObjectBypassHook, EditorUI) {
        ALL_DELEGATES_AND_SAFE_PRIO("creator.customobjectbypass")

        void onNewCustomItem(CCObject* sender) {
            if (auto gameManager = utils::get<GameManager>()) {
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
