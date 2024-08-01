#include <Geode/Geode.hpp>
#include <Geode/loader/SettingEvent.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <modules/config/config.hpp>
#include <modules/gui/layouts/window/window.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>
#include <imgui-cocos.hpp>

using namespace eclipse;

static bool s_isInitialized = false;

class $modify(EclipseButtonMLHook, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

#ifdef GEODE_IS_ANDROID
        // Temporarily add a button to toggle the GUI on android. (This will be removed later)
        auto androidButton = CCMenuItemSpriteExtra::create(
            cocos2d::CCSprite::createWithSpriteFrameName("GJ_everyplayBtn_001.png"),
            this, menu_selector(EclipseButtonMLHook::onToggleUI)
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

        s_isInitialized = true;

        return true;
    }

    void onToggleUI(CCObject* sender) {
        gui::Engine::get()->toggle();
        config::save();
    }
};

class $modify(HackSchedulerHook, cocos2d::CCScheduler) {
    static void onModify(auto& self) {
        FIRST_PRIORITY("cocos2d::CCScheduler::update");
    }

    void update(float dt) override {
        cocos2d::CCScheduler::update(dt);

        for (const auto& hack : hack::Hack::getHacks())
            hack->update();

        // Add ability for ImGui to capture right click
        if (s_isInitialized) {
            auto& io = ImGui::GetIO();
            if (keybinds::isKeyPressed(keybinds::Keys::MouseRight)) {
                io.AddMouseButtonEvent(1, true);
            } else if (keybinds::isKeyReleased(keybinds::Keys::MouseRight)) {
                io.AddMouseButtonEvent(1, false);
            }
        }

        keybinds::Manager::get()->update();
    }
};

class $modify(EclipseUILayerHook, UILayer) {
    bool init(GJBaseGameLayer* p0) {
        if (!UILayer::init(p0)) return false;
        auto menu = cocos2d::CCMenu::create();
        menu->setID("eclipse-ui"_spr);
        this->addChild(menu, 1000);
        menu->setPosition({0, 0});
        return true;
    }
};

$on_mod(Loaded) {
    // Allow user to change between OpenGL 2.0/3.0
    auto* mod = geode::Mod::get();
    ImGuiCocos::get().setForceLegacy(mod->getSettingValue<bool>("legacy-render"));
    geode::listenForSettingChanges<bool>("legacy-render", [](bool value) {
        ImGuiCocos::get().setForceLegacy(value);
    });

    // Load the configuration file.
    config::load();

    // Initialize the hacks.
    hack::Hack::initializeHacks();

    // Load keybinds
    keybinds::Manager::get()->init();
}
