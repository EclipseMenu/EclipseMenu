#include <Geode/loader/Setting.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/CCScheduler.hpp>

#include <modules/config/config.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>
#include <modules/gui/blur/blur.hpp>
#include <modules/gui/float-button.hpp>
#include <modules/gui/theming/manager.hpp>
#include <modules/debug/trace.hpp>
#include <imgui-cocos.hpp>

using namespace eclipse;

static bool s_isInitialized = false;

static void toggleMenu(bool down = true) {
    gui::Engine::get()->toggle();
    config::save();
    gui::ThemeManager::get()->saveTheme();
}

class $modify(EclipseButtonMLHook, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        // {
        //     auto menu = this->getChildByID("bottom-menu");
        //     auto rendererSwitchButton = CCMenuItemSpriteExtra::create(
        //         cocos2d::CCSprite::createWithSpriteFrameName("GJ_editModeBtn_001.png"),
        //         this, menu_selector(EclipseButtonMLHook::onToggleRenderer)
        //     );
        //     rendererSwitchButton->setID("render-switch"_spr);
        //     menu->addChild(rendererSwitchButton);
        //     menu->updateLayout();
        // }

        if (s_isInitialized) return true;

        // Compile blur shader
        gui::blur::init();

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        #ifdef GEODE_IS_MOBILE
        // This will create the floating button and keep it across scenes
        gui::FloatingButton::get()->setCallback([]{ toggleMenu(); });
        #endif

        // Register the keybind
        auto& key = keybinds::Manager::get()->registerKeybind("menu.toggle", "Toggle UI", toggleMenu);
        config::setIfEmpty("menu.toggleKey", keybinds::Keys::Tab);
        key.setKey(config::get<keybinds::Keys>("menu.toggleKey", keybinds::Keys::Tab));
        key.setInitialized(true);
        hack::Hack::lateInitializeHacks();

        s_isInitialized = true;

        return true;
    }

    void onToggleRenderer(CCObject* sender) {
        auto type = gui::Engine::getRendererType() == gui::RendererType::ImGui
            ? gui::RendererType::Cocos2d
            : gui::RendererType::ImGui;
        gui::ThemeManager::get()->setRenderer(type);
    }
};

class HackUpdater : public cocos2d::CCObject {
public:
    static HackUpdater* get() {
        static HackUpdater instance;
        return &instance;
    }

    void update(float dt) override {
        for (const auto& hack : hack::Hack::getHacks())
            hack->update();

        // Add ability for ImGui to capture right click
        if (s_isInitialized && gui::Engine::getRendererType() == gui::RendererType::ImGui) {
            auto& io = ImGui::GetIO();
            if (keybinds::isKeyPressed(keybinds::Keys::MouseRight)) {
                io.AddMouseButtonEvent(1, true);
            } else if (keybinds::isKeyReleased(keybinds::Keys::MouseRight)) {
                io.AddMouseButtonEvent(1, false);
            }
        }

        keybinds::Manager::get()->update();
        gui::blur::update(dt);
    }
};

$on_mod(Loaded) {
    // Allow user to change disable VBO (resolves issues on older hardware)
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

    gui::ThemeManager::get();

    // Add "Interface" tab to edit theme settings
    {
        using namespace gui;
        auto tab = MenuTab::find("Interface");
        std::vector<std::string> themeNames = {};
        for (ThemeMeta theme : ThemeManager::get()->listAvailableThemes()) {
            themeNames.push_back(theme.name);
        }
        if (!themeNames.empty()) {
            auto themeCombo = tab->addCombo("Theme", "themeIndex", themeNames, 0);
            themeCombo->callback([](int value) {
                ThemeManager::get()->loadTheme(ThemeManager::get()->listAvailableThemes()[value].path);
                ThemeManager::get()->setUIScale(config::getTemp<float>("uiScale", 1.f));
            });
        }
        tab->addInputFloat("UI Scale", "uiScale", 0.75f, 2.f, "x%.3f")
            ->callback([](float value) {
                ThemeManager::get()->setUIScale(value);
            })->disableSaving();

        auto fontCombo = tab->addCombo("Font", "fontIndex", ThemeManager::getFontNames(), 0);
        fontCombo->callback([](int value) {
            ThemeManager::get()->setSelectedFont(value);
        })->disableSaving();
        tab->addInputFloat("Font Size", "fontSize", 10.f, 64.f)
            ->callback([](float value) {
                if (value >= 10.f) ThemeManager::get()->setFontSize(value);
            })->disableSaving();
        tab->addButton("Reload Fonts")->callback([fontCombo] {
            ImGuiCocos::get().reload();
            fontCombo->setItems(ThemeManager::getFontNames());
        });

        tab->addCombo("Layout Type", "layout", {"Tabbed", "Panel", "Sidebar"}, 0)
            ->callback([](int value) {
                ThemeManager::get()->setLayoutMode(static_cast<imgui::LayoutMode>(value));
            })->disableSaving();

        tab->addCombo("Style", "style", imgui::THEME_NAMES, 0)
            ->callback([](int value) {
                ThemeManager::get()->setComponentTheme(static_cast<imgui::ComponentTheme>(value));
            })->disableSaving();

        auto blurToggle = tab->addToggle("Enable blur", "blurEnabled")
            ->callback([](bool value) { ThemeManager::get()->setBlurEnabled(value); });
        blurToggle->addOptions([](auto opt) {
            opt->addInputFloat("Blur speed", "blurSpeed", 0.f, 10.f, "%.3f s")
                ->callback([](float value){ ThemeManager::get()->setBlurSpeed(value); })
                ->disableSaving();
        });
        blurToggle->disableSaving();

        auto accentColor = tab->addColorComponent("Accent Color", "accent", true);
        accentColor->callback([](const Color& color) {
            ThemeManager::get()->applyAccentColor(color);
        })->disableSaving();

        auto backgroundColor = tab->addColorComponent("Background Color", "background", true);
        backgroundColor->callback([](const Color& color) {
            ThemeManager::get()->applyBackgroundColor(color);
        })->disableSaving();

        auto searchInput = tab->addInputText("Search", "search");
        searchInput->callback([](std::string input) {
            static bool hasSearched = false;

            if (input.empty())
            {
                if (hasSearched)
                {
                    for (auto& tab : Engine::get()->getTabs())
                    {
                        tab->setSearchedFor(false);

                        for (auto& component : tab->getComponents())
                            component->setSearchedFor(false);
                    }

                    hasSearched = false;
                }
            }
            else
            {
                hasSearched = true;

                for (auto& tab : Engine::get()->getTabs())
                {
                    bool hasFoundComponent = false;

                    for (auto& component : tab->getComponents())
                    {
                        if (utils::matchesStringFuzzy(component->getTitle(), input))
                        {
                            component->setSearchedFor(true);
                            hasFoundComponent = true;
                        }
                        else
                            component->setSearchedFor(false);
                    }

                    tab->setSearchedFor(hasFoundComponent);
                }
            }
        })->disableSaving();
    }

    // Schedule hack updates
    cocos2d::CCScheduler::get()->scheduleSelector(
        schedule_selector(HackUpdater::update),
        HackUpdater::get(), 0.f, false
    );
}