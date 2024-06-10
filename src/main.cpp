#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <modules/config/config.hpp>
#include <modules/gui/imgui/imgui.hpp>
#include <modules/hack/hack.hpp>

using namespace eclipse;

class $modify(MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        return true;
    }
};

class $modify(cocos2d::CCScheduler) {
    void update(float dt) override {
        cocos2d::CCScheduler::update(dt);
        for (auto hack : hack::Hack::getHacks()) {
            hack->update();
        }
    }
};

$on_mod(Loaded) {
    // Load the configuration file.
    config::load();
}