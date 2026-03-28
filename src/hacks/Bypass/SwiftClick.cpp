#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

#ifdef GEODE_IS_MOBILE

namespace eclipse::hacks::Bypass {
    class $hack(SwiftClick) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.swiftclick")->handleKeybinds()->setDescription();
        }
        [[nodiscard]] bool isCheating() const override { return config::get<"bypass.swiftclick", bool>(); }
        [[nodiscard]] const char* getId() const override { return "Swift Click"; }
    };

    REGISTER_HACK(SwiftClick)

    class $modify(SwiftClickGJBGLHook, GJBaseGameLayer) {
        ADD_HOOKS_DELEGATE("bypass.swiftclick")

        void handleButton(bool down, int button, bool isPlayer1) {
            m_allowedButtons.clear();
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
    };
}
#endif