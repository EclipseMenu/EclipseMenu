#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameObject.hpp>

namespace eclipse::hacks::Level {
    static std::unordered_set<int> s_objectFilter = { 221, 743, 744, 899, 900, 915, 1006, 32, 33, 899, 900, 29, 30, 104, 105, 717, 718, 1007, 1520, 2903, 3029, 3030, 3031, 2999, 3606, 3612, 3009, 3010, 3014, 3015, 3021, 3020, 1818, 1819, 32, 33, 1612, 1613, 3608};

    class $hack(LayoutMode) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.layoutmode")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"level.layoutmode", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Layout Mode"; }
    };

    REGISTER_HACK(LayoutMode)

    class $modify(LayoutModePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.layoutmode")

        /*void resetLevel() {
            PlayLayer::resetLevel();
            if (m_background) {
                m_background->setColor({40, 125, 255});
            }
            if (m_groundLayer) {
                m_groundLayer->updateGround01Color({0, 102, 255});
                m_groundLayer->updateGround02Color({0, 102, 255});
            }
            if (m_groundLayer2) {
                m_groundLayer2->updateGround01Color({0, 102, 255});
                m_groundLayer2->updateGround02Color({0, 102, 255});
            }
            if (m_middleground) {
                m_middleground->updateGroundColor({40, 125, 255}, true);
            }
        }*/

        void addObject(GameObject* obj) {
            if (s_objectFilter.contains(obj->m_objectID)) {
                obj->m_isHide = true;
                return;
            } else {
                obj->m_hasNoGlow = true;
                obj->m_activeMainColorID = -1;
                obj->m_activeDetailColorID = -1;
                obj->m_detailUsesHSV = false;
                obj->m_baseUsesHSV = false;
                obj->m_isHide = false;
                obj->setOpacity(255);
                PlayLayer::addObject(obj);
            }
        }
    };

    class $modify(LayoutModeGOHook, GameObject) {
        ADD_HOOKS_DELEGATE("level.layoutmode")

        void addGlow(gd::string p0) {
            GameObject::addGlow(p0);
            if ((m_objectType == GameObjectType::Decoration && m_objectID != 44 && m_objectID != 38 && m_objectID != 749 && m_objectID != 747) || m_isNoTouch) { // 44 being practice mode checkpoint, because thats a "decoration"
                m_isHide = true;
            } else {
                m_isHide = false;
            }

        }

        // i have to add this or we get creepypasta from ashley wave trials
        void setVisible(bool v) {
            if ((m_objectType == GameObjectType::Decoration && m_objectID != 44 && m_objectID != 38 && m_objectID != 749 && m_objectID != 747) || m_isNoTouch) { // 44 being practice mode checkpoint, because thats a "decoration"
                GameObject::setVisible(false);
            } else {
                GameObject::setVisible(v);
            }
        }
    };
    
}
