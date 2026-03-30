#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#ifdef GEODE_IS_MOBILE

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(SwiftClick) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.swiftclick")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] const char* getId() const override { return "Swift Click"; }
        [[nodiscard]] bool isCheating() const override {
            return config::get<"bypass.swiftclick", bool>();
        }
    };

    REGISTER_HACK(SwiftClick)

    class $modify(SwiftClickGJBGLHook, GJBaseGameLayer) {
        static void onModify(auto& self) {
            constexpr std::array autotoggles = std::to_array<std::string_view>({
                "bypass.swiftclick",
                "player.autoclick",
                "player.framestepper"
            });

            auto hook = self.getHook("GJBaseGameLayer::handleButton").unwrapOrDefault();
            if (!hook) return;

            for (auto toggle : autotoggles) {
                config::addDelegate(toggle, [hook] {
                    hook->setEnabled(std::ranges::any_of(autotoggles, [](auto toggle) {
                        return config::get<bool>(toggle, false);
                    }));
                });
            }
        }

        void handleButton(bool down, int button, bool isPlayer1) {
            m_allowedButtons.clear();
            GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
    };
}

#endif