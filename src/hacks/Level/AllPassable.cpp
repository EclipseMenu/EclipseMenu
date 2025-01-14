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
            bool m_checked = false;
        };
    };

    class $modify(AllPassPLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("level.allpassable")

        void setupHasCompleted() {
            PlayLayer::setupHasCompleted();
            for (auto obj : geode::cocos::CCArrayExt<AllPassGOHook*>(m_objects)) {
                auto fields = obj->m_fields.self();
                fields->m_isPassable = obj->m_isPassable;
                fields->m_checked = true;
                obj->m_isPassable = true;
            }
        }
    };

    class $hack(AllPassable) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.allpassable")
               ->setDescription()
               ->handleKeybinds()
               ->callback([] (bool toggled) {
                if (auto pl = utils::get<PlayLayer>()) {
                    for (auto obj : geode::cocos::CCArrayExt<AllPassGOHook*>(pl->m_objects)) {
                        auto fields = obj->m_fields.self();
                        if (!fields->m_checked) {
                            fields->m_isPassable = obj->m_isPassable;
                            fields->m_checked = true;
                        }
                        obj->m_isPassable = toggled || fields->m_isPassable;
                    }
                }
               });
        }

        [[nodiscard]] bool isCheating() const override { RETURN_CACHED_BOOL("level.allpassable"); }
        [[nodiscard]] const char* getId() const override { return "All Passable"; }
    };

    REGISTER_HACK(AllPassable)
}