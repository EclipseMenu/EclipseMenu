#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <modules/config/config.hpp>
#include <modules/gui/imgui/imgui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

using namespace eclipse;

static bool s_isInitialized = false;

class $modify(MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        if (s_isInitialized) return true;

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        // Register the keybind
        auto& key = keybinds::Manager::get()->registerKeybind("menu.toggle", "Toggle UI", []() {
            gui::Engine::get()->toggle();
            config::save();
        });
        key.setKey(keybinds::Keys::Tab);
        key.setInitialized(true);

        hack::Hack::lateInitializeHacks();

        // TODO: Load saved keybind states

        s_isInitialized = true;

        return true;
    }
};

class $modify(cocos2d::CCScheduler) {
    void update(float dt) override {
        cocos2d::CCScheduler::update(dt);
        for (auto hack : hack::Hack::getHacks()) {
            hack->update();
        }
        keybinds::Manager::get()->update();
    }
};

$on_mod(Loaded) {
    // Load the configuration file.
    config::load();

    // Initialize the hacks.
    hack::Hack::initializeHacks();
}