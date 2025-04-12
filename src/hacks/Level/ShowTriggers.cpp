#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(ShowTriggers) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.showtriggers")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"level.showtriggers", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Show Triggers"; }
    };

    REGISTER_HACK(ShowTriggers)

    // :(
    // GameObject->m_objectType is set during GameObject::customSetup()
    // so we have to check the object ID manually instead
    static const std::unordered_set<int> triggerIds = {
        22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 55, 56,
        57, 58, 59, 105, 744, 899, 900, 901, 915, 1006, 1007,
        1049, 1268, 1346, 1347, 1520, 1585, 1595, 1611, 1612, 1613,
        1616, 1811, 1812, 1814, 1815, 1817, 1818, 1819, 1912, 1913,
        1914, 1915, 1916, 1917, 1931, 1932, 1934, 1935, 2015, 2016,
        2062, 2066, 2067, 2068, 2899, 2900, 2901, 2904, 2905, 2907,
        2909, 2910, 2911, 2912, 2913, 2914, 2915, 2916, 2917, 2919,
        2920, 2921, 2922, 2923, 2924, 2925, 2999, 3016, 3017, 3018,
        3019, 3020, 3021, 3022, 3023, 3024, 3029, 3030, 3031, 3033,
        3600, 3602, 3603, 3604, 3605, 3606, 3607, 3608, 3609, 3612,
        3613, 3614, 3615, 3617, 3618, 3619, 3620, 3640, 3641, 3642,
        3655, 3660, 3661, 3662, 3643, 3006, 3007, 3008, 3009, 3010,
        3011, 3012, 3013, 3014, 3015, 2903
    };

    class $modify(GameObject) {
        ADD_HOOKS_DELEGATE("level.showtriggers")

        void customSetup() override {
            bool editorEnabled = this->m_editorEnabled;
            int id = this->m_objectID;
            this->m_editorEnabled = editorEnabled || triggerIds.contains(id);

            GameObject::customSetup();
            this->m_editorEnabled = editorEnabled;
        }
    };

    static GameObject* s_lastEditedTrigger;
    static int s_originalSectionIndex;

    class $modify(PlayLayer) {
        void addObject(GameObject* obj) {
            if (!obj) return PlayLayer::addObject(obj);

            /*
             * // original code
             * if (m_objectID == 3613 || m_objectID == 3662
             *  || obj->getType(obj) == Modifier && (obj->m_unk4D0 != 1 || !obj->m_isTouchTriggered) && obj->m_objectID != 2063) {
             *     obj->m_outerSectionIndex = -1;
             * } else {
             *     this->addToSection(this, obj);
             * }
             */
            bool isTrigger = obj->m_objectType == GameObjectType::Modifier;
            bool idsCheck = obj->m_objectID == 3613 || obj->m_objectID == 3662;
            if (isTrigger && !idsCheck && config::get<bool>("level.showtriggers", false)) {
                if ((obj->m_classType != GameObjectClassType::Effect || !static_cast<EffectGameObject*>(obj)->m_isTouchTriggered) && obj->m_objectID != 2063) {
                    s_lastEditedTrigger = obj; // this will queue the trigger to be added to the section
                    s_originalSectionIndex = obj->m_outerSectionIndex;
                }
            }

            PlayLayer::addObject(obj);
        }
    };

    class $modify(GJBaseGameLayer) {
        // this function appears just after that if statement in PlayLayer::addObject
        void addToGroups(GameObject* obj, bool p1) {
            // manually add the trigger to section and restore the original section index
            if (obj == s_lastEditedTrigger) {
                obj->m_outerSectionIndex = s_originalSectionIndex;
                GJBaseGameLayer::addToSection(obj);
                s_lastEditedTrigger = nullptr;
            }
            GJBaseGameLayer::addToGroups(obj, p1);
        }
    };
}
