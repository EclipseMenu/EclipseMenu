#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <modules/config/config.hpp>
#include <modules/gui/imgui/imgui.hpp>

using namespace eclipse;

class $modify(MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        return true;
    }
};

$on_mod(Loaded) {
    // Load the configuration file.
    config::load();
}