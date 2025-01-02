#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>

namespace eclipse::hacks::Creator {
    static std::vector<GameObject*> s_editorTriggers;

    class HideTriggers : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.creator");
            tab->addToggle("creator.hidetriggers")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Hide Triggers"; }
    };

    REGISTER_HACK(HideTriggers)

    static bool isSpeedPortal(GameObject* obj) {
        auto id = obj->m_objectID;
        return id == 200 || id == 201 || id == 202 || id == 203 || id == 1334;
    }

    class $modify(HideTriggersEUIHook, EditorUI) {
        ADD_HOOKS_DELEGATE("creator.hidetriggers")

        void onPlaytest(cocos2d::CCObject* sender) {
            EditorUI::onPlaytest(sender);

            auto* editorLayer = utils::get<LevelEditorLayer>();
            if (!editorLayer) return;

            // Store all triggers
            s_editorTriggers.clear();
            geode::cocos::CCArrayExt<GameObject*> objects = editorLayer->m_objects;
            for (auto obj : objects) {
                if (obj->m_objectType == GameObjectType::Modifier && !isSpeedPortal(obj) && obj->m_objectID != 2063) {
                    s_editorTriggers.push_back(obj);
                }
            }
        }
    };

    class $modify(HideTriggersLELHook, LevelEditorLayer) {
        ADD_HOOKS_DELEGATE("creator.hidetriggers")

        void onStopPlaytest() {
            LevelEditorLayer::onStopPlaytest();

            // Show all triggers
            for (auto obj : s_editorTriggers)
                obj->setVisible(true);

            s_editorTriggers.clear();
        }

        void updateVisibility(float dt) override {
            LevelEditorLayer::updateVisibility(dt);

            // Hide all triggers
            for (auto obj : s_editorTriggers)
                obj->setVisible(false);
        }
    };
}
