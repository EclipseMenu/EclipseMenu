#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameObject.hpp>

namespace eclipse::hacks::Player {
    class $modify(AllPassGOHook, GameObject) {
        struct Fields {
            bool m_isPassable = false;
        };

        ENABLE_SAFE_HOOKS_ALL()

        void customSetup() {
            GameObject::customSetup();

            m_fields->m_isPassable = m_isPassable;
            // Because in the editor, uh stuff might get messed up
            if (PlayLayer::get()) {
                m_isPassable = config::get<bool>("level.allpassable", true) || m_fields->m_isPassable;
            }
        }
    };

    class AllPassable : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            tab->addToggle("level.allpassable")
               ->setDescription()
               ->handleKeybinds()
               ->callback([this] (bool toggled) {
                if (auto pl = PlayLayer::get()) {
                    for (auto obj : geode::cocos::CCArrayExt<AllPassGOHook>(pl->m_objects)) {
                        obj->m_isPassable = config::get<bool>("level.allpassable", true) || obj->m_fields->m_isPassable;
                    }
                }
               });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("level.allpassable", false); }
        [[nodiscard]] const char* getId() const override { return "All Passable"; }
    };

    REGISTER_HACK(AllPassable)
}