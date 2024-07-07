#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <modules/config/config.hpp>
#include <modules/gui/themes/megahack/megahack.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

using namespace eclipse;

static bool s_isInitialized = false;

class $modify(MyMenuLayer, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

#ifdef GEODE_IS_ANDROID
        // Temporarily add a button to toggle the GUI on android. (This will be removed later)
        auto androidButton = CCMenuItemSpriteExtra::create(
            cocos2d::CCSprite::createWithSpriteFrameName("GJ_everyplayBtn_001.png"),
            this, menu_selector(MyMenuLayer::onToggleUI)
        );
        androidButton->setID("toggle"_spr);
        auto menu = this->getChildByID("bottom-menu");
        menu->addChild(androidButton);
        menu->updateLayout();
#endif

        if (s_isInitialized) return true;

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        // Register the keybind
        auto& key = keybinds::Manager::get()->registerKeybind("menu.toggle", "Toggle UI", []() {
            gui::Engine::get()->toggle();
            config::save();
        });
        config::setIfEmpty("menu.toggleKey", keybinds::Keys::Tab);
        key.setKey(config::get<keybinds::Keys>("menu.toggleKey"));
        key.setInitialized(true);

        hack::Hack::lateInitializeHacks();

        // TODO: Load saved keybind states

        s_isInitialized = true;

        return true;
    }

    void onToggleUI(CCObject* sender) {
        gui::Engine::get()->toggle();
        config::save();
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

    // Load keybinds UI
    keybinds::Manager::get()->setupTab();

    // Initialize the hacks.
    hack::Hack::initializeHacks();
}