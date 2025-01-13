#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Player {
    class $modify(AllPassGOHook, GameObject) {
        struct Fields {
            bool m_isPassable = false;
            bool checked = false;
        };
    };

    class $modify(AllPassPLHook, PlayLayer) {
        void setupHasCompleted() {
            PlayLayer::setupHasCompleted();
            if (!config::get<bool>("level.allpassable", false)) return;
            for (auto obj : geode::cocos::CCArrayExt<AllPassGOHook *>(m_objects)) {
                obj->m_fields->m_isPassable = obj->m_isPassable;
                obj->m_fields->checked = true;
                obj->m_isPassable = true;
            }
        }
    };

    class AllPassable : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.allpassable")
               ->setDescription()
               ->handleKeybinds()
               ->callback([] (bool toggled) {
                if (auto pl = utils::get<PlayLayer>()) {
                    for (auto obj : geode::cocos::CCArrayExt<AllPassGOHook *>(pl->m_objects)) {
                        if (!obj->m_fields->checked) {
                            obj->m_fields->m_isPassable = obj->m_isPassable;
                            obj->m_fields->checked = true;
                        }
                        obj->m_isPassable = config::get<bool>("level.allpassable", false) || obj->m_fields->m_isPassable;
                    }
                }
               });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("level.allpassable", false); }
        [[nodiscard]] const char* getId() const override { return "All Passable"; }
    };

    REGISTER_HACK(AllPassable)
}