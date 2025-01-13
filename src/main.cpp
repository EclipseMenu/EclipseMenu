#include <Geode/loader/Setting.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <imgui-cocos.hpp>
#include <modules/config/config.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

#include <modules/gui/float-button.hpp>
#include <modules/gui/blur/blur.hpp>

#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/theming/manager.hpp>

#include <modules/i18n/DownloadPopup.hpp>
#include <modules/i18n/translations.hpp>

#include <modules/gui/components/button.hpp>
#include <modules/gui/components/color.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/gui/components/input-float.hpp>
#include <modules/gui/components/input-text.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/gui/imgui/animation/easing.hpp>

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

        #ifdef ECLIPSE_DEBUG_BUILD
        {
            auto menu = this->getChildByID("bottom-menu");
            auto rendererSwitchButton = CCMenuItemSpriteExtra::create(
                cocos2d::CCSprite::createWithSpriteFrameName("GJ_editModeBtn_001.png"),
                this, menu_selector(EclipseButtonMLHook::onToggleRenderer)
            );
            rendererSwitchButton->setID("render-switch"_spr);
            menu->addChild(rendererSwitchButton);
            menu->updateLayout();
        }
        #endif

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

    // Load language files
    i18n::init();

    // Initialize the hacks.
    hack::Hack::initializeHacks();

    // Load keybinds
    keybinds::Manager::get()->init();

    gui::ThemeManager::get();

    // Add bitmap fonts to texture search path
    auto bmfontPath = geode::Mod::get()->getConfigDir() / "bmfonts";
    std::filesystem::create_directories(bmfontPath / GEODE_MOD_ID);
    utils::get<cocos2d::CCFileUtils>()->addSearchPath(bmfontPath.string().c_str());
    geode::log::info("Added bitmap fonts to search path: {}", bmfontPath.string());

    // Add "Interface" tab to edit theme settings
    {
        using namespace gui;
        auto tab = MenuTab::find("tab.interface");
        std::vector<std::string> themeNames = {};
        for (ThemeMeta theme : ThemeManager::get()->listAvailableThemes()) {
            themeNames.push_back(theme.name);
        }
        if (!themeNames.empty()) {
            auto themeCombo = tab->addCombo("interface.theme", "themeIndex", themeNames, 0);
            themeCombo->callback([](int value) {
                ThemeManager::get()->loadTheme(ThemeManager::get()->listAvailableThemes()[value].path);
                ThemeManager::get()->setUIScale(config::getTemp<float>("uiScale", 1.f));
            });
        }
        tab->addInputFloat("interface.ui-scale", "uiScale", 0.75f, 2.f, "x%.3f")
           ->callback([](float value) {
               ThemeManager::get()->setUIScale(value);
           })->disableSaving()->setFlags(ComponentFlags::DisableCocos);

        auto fontCombo = tab->addCombo("interface.font", "fontIndex", ThemeManager::getFontNames(), 0);
        fontCombo->callback([](int value) {
            ThemeManager::get()->setSelectedFont(value);
        })->disableSaving()->setFlags(ComponentFlags::DisableCocos);
        tab->addInputFloat("interface.font-size", "fontSize", 10.f, 64.f)
           ->callback([](float value) {
               if (value >= 10.f) ThemeManager::get()->setFontSize(value);
           })->disableSaving()->setFlags(ComponentFlags::DisableCocos);
        tab->addButton("interface.reload-fonts")->callback([fontCombo] {
            ImGuiCocos::get().reload();
            fontCombo->setItems(ThemeManager::getFontNames());
        })->setFlags(ComponentFlags::DisableCocos);

        tab->addCombo("interface.layout-type", "layout", {"Tabbed", "Panel", "Sidebar"}, 0)
           ->callback([](int value) {
               ThemeManager::get()->setLayoutMode(static_cast<imgui::LayoutMode>(value));
           })->disableSaving()->setFlags(ComponentFlags::DisableCocos);

        tab->addCombo("interface.style", "style", imgui::THEME_NAMES, 0)
           ->callback([](int value) {
               ThemeManager::get()->setComponentTheme(static_cast<imgui::ComponentTheme>(value));
           })->disableSaving()->setFlags(ComponentFlags::DisableCocos);

        auto blurToggle = tab->addToggle("interface.enable-blur", "blurEnabled")
                             ->callback([](bool value) { ThemeManager::get()->setBlurEnabled(value); });
        blurToggle->addOptions([](auto opt) {
            opt->addInputFloat("interface.blur-speed", "blurSpeed", 0.f, 10.f, "%.3f s")
               ->callback([](float value) { ThemeManager::get()->setBlurSpeed(value); })
               ->disableSaving();
        });
        blurToggle->disableSaving();
        blurToggle->setFlags(ComponentFlags::DisableCocos);

        config::setIfEmpty<bool>("menu.animateWindows", true);
        config::setIfEmpty("menu.animationDuration", 0.3);
        config::setIfEmpty("menu.animationEasingType", animation::Easing::Quadratic);
        config::setIfEmpty("menu.animationEasingMode", animation::EasingMode::EaseInOut);
        auto animateToggle = tab->addToggle("menu.animateWindows")->setDescription();
        animateToggle->addOptions([](auto opt) {
                opt->addInputFloat("menu.animationDuration", 0.f, 10.f, "%.3f s");
                opt->addCombo("menu.animationEasingType", {
                    i18n::get_("menu.animationEasingType.0"),
                    i18n::get_("menu.animationEasingType.1"),
                    i18n::get_("menu.animationEasingType.2"),
                    i18n::get_("menu.animationEasingType.3"),
                    i18n::get_("menu.animationEasingType.4"),
                    i18n::get_("menu.animationEasingType.5"),
                    i18n::get_("menu.animationEasingType.6"),
                    i18n::get_("menu.animationEasingType.7"),
                    i18n::get_("menu.animationEasingType.8"),
                    i18n::get_("menu.animationEasingType.9"),
                    i18n::get_("menu.animationEasingType.10"),
                }, 2);
                opt->addCombo("menu.animationEasingMode", {
                    i18n::get_("menu.animationEasingMode.0"),
                    i18n::get_("menu.animationEasingMode.1"),
                    i18n::get_("menu.animationEasingMode.2"),
                }, 2);
            });
        animateToggle->setFlags(ComponentFlags::OnlyTabbed);

        auto accentColor = tab->addColorComponent("interface.accent-color", "accent", true);
        accentColor->callback([](const Color& color) {
            ThemeManager::get()->applyAccentColor(color);
        })->disableSaving();

        auto backgroundColor = tab->addColorComponent("interface.background-color", "background", true);
        backgroundColor->callback([](const Color& color) {
            ThemeManager::get()->applyBackgroundColor(color);
        })->disableSaving();

        auto languageCombo = tab->addCombo(
            "interface.language", "language.index",
            i18n::getAvailableLanguages(), i18n::getLanguageIndex()
        );
        languageCombo->callback([languageCombo](int value) {
            if (Engine::getRendererType() == RendererType::Cocos2d) {
                auto meta = i18n::fetchAvailableLanguages()[value];
                if (i18n::hasBitmapFont(meta.charset)) {
                    i18n::setLanguage(i18n::fetchAvailableLanguages()[value].code);
                    config::set("language", i18n::getCurrentLanguage());
                    languageCombo->setItems(i18n::getAvailableLanguages());
                } else {
                    auto oldIndex = i18n::getLanguageIndex();
                    config::setTemp("language.index", oldIndex);
                    Popup::create(
                        i18n::get_("common.warning"),
                        i18n::get_("interface.font-not-found"),
                        i18n::get_("common.yes"),
                        i18n::get_("common.no"),
                        [charset = std::move(meta.charset)](bool yes) {
                            if (!yes) return;
                            i18n::DownloadPopup::create(charset)->show();
                        }
                    );
                }
            } else {
                i18n::setLanguage(i18n::fetchAvailableLanguages()[value].code);
                config::set("language", i18n::getCurrentLanguage());

                languageCombo->setItems(i18n::getAvailableLanguages());

                // Reload fonts if in ImGui mode
                ImGuiCocos::get().reload();
            }
        })->disableSaving();

        auto searchInput = tab->addInputText("interface.search", "search");
        searchInput->callback([](std::string input) {
            static bool hasSearched = false;

            if (input.empty()) {
                if (hasSearched) {
                    for (auto& tab : Engine::get()->getTabs()) {
                        tab->setSearchedFor(false);

                        for (auto& component : tab->getComponents())
                            component->setSearchedFor(false);
                    }

                    hasSearched = false;
                }
            } else {
                hasSearched = true;

                for (auto& tab : Engine::get()->getTabs()) {
                    bool hasFoundComponent = false;

                    for (auto& component : tab->getComponents()) {
                        if (utils::matchesStringFuzzy(i18n::get(component->getTitle()), input)) {
                            component->setSearchedFor(true);
                            hasFoundComponent = true;
                        } else component->setSearchedFor(false);
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
