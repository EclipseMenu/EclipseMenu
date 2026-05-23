#include <Core/Config.hpp>
#include <Core/InputManager.hpp>
#include <UI/HeliosLayer.hpp>

using namespace eclipse::prelude;

// template <>
// bool eclipse::default_value<bool, "player.noclip">() { return true; }
//
// $execute {
//     auto& noclip = config::ref<bool, "player.noclip">();
//     auto& also_noclip = ConfigSetting<bool, "player.noclip">::get();
//
//     noclip.listen([](bool enabled) {
//         log::info("Noclip {}", enabled ? "enabled" : "disabled");
//     });
//
//     also_noclip.set(true);
//
//     if (config::get<bool, "player.noclip">()) {
//         config::listen<bool, "player.noclip">([](bool enabled) {
//             // second listener
//         });
//     }
//
//     config::set<bool, "player.noclip">(false);
//
//     auto value = Registry::get()->find<bool>("player.noclip").unwrap();
//     value->set(true);
// }

$on_game(Loaded) {
    // Create and attach the UI interface
    (void) HeliosLayer::get();

    auto inputManager = InputManager::get();
    inputManager->setDefaultKeybind("toggle-ui", KEY_Tab);
    inputManager->registerListener(
        "toggle-ui",
        [](KeybindMode, bool isDown, double) -> bool {
            if (!isDown) return false;
            if (auto layer = HeliosLayer::get()) {
                layer->toggle();
            }
            return false;
        }
    );
}