#include <Geode/loader/Setting.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/MenuLayer.hpp>

#include <imgui-cocos.hpp>
#include <modules/config/config.hpp>
#include <modules/hack/hack.hpp>
#include <modules/keybinds/manager.hpp>

#include <modules/gui/float-button.hpp>
#include <modules/gui/blur/blur.hpp>

#include <AdvancedLabel.hpp>
#include <modules/gui/cocos/cocos.hpp>
#include <modules/gui/theming/manager.hpp>

#include <modules/i18n/DownloadPopup.hpp>
#include <modules/i18n/translations.hpp>

#include <modules/gui/components/button.hpp>
#include <modules/gui/components/color.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/gui/components/input-float.hpp>
#include <modules/gui/components/input-text.hpp>
#include <modules/gui/components/label.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/gui/imgui/animation/easing.hpp>

using namespace eclipse;

static bool s_isInitialized = false;

static void toggleMenu(bool down = true) {
    gui::Engine::get()->toggle();
    config::save();
    gui::ThemeManager::get()->saveTheme();
}

class $modify(ClearCacheGMHook, GameManager) {
    void reloadAllStep5() {
        GameManager::reloadAllStep5();
        utils::purgeAllSingletons();
        gui::blur::cleanup();
        #ifdef ECLIPSE_USE_FLOATING_BUTTON
        gui::FloatingButton::get()->reloadSprite();
        #endif
    }
};

class $modify(EclipseButtonMLHook, MenuLayer) {
    bool init() override {
        if (!MenuLayer::init()) return false;

        // #ifdef ECLIPSE_DEBUG_BUILD
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
        // #endif

        if (s_isInitialized) return true;

        // Compile blur shader
        gui::blur::init();

        // Initialize the GUI engine.
        gui::Engine::get()->init();

        #ifdef ECLIPSE_USE_FLOATING_BUTTON
        // This will create the floating button and keep it across scenes
        gui::FloatingButton::get()->setCallback([]{ toggleMenu(); });
        #endif

        // Register the keybind
        auto& key = keybinds::Manager::get()->registerKeybind("menu.toggle", "Toggle UI", toggleMenu);
        config::setIfEmpty("menu.toggleKey", keybinds::Keys::Tab);
        key.setKey(config::get<keybinds::Keys>("menu.toggleKey", keybinds::Keys::Tab));
        key.setInitialized(true);
        hack::lateInitializeHacks();

        s_isInitialized = true;

        return true;
    }

    // void onToggleRenderer(CCObject* sender) {
    //     auto type = gui::Engine::getRendererType() == gui::RendererType::ImGui
    //                     ? gui::RendererType::Cocos2d
    //                     : gui::RendererType::ImGui;
    //     gui::ThemeManager::get()->setRenderer(type);
    // }
};

class HackUpdater : public cocos2d::CCObject {
public:
    static HackUpdater* create() {
        auto ret = new HackUpdater();
        ret->autorelease();
        return ret;
    }

    void update(float dt) override {
        for (const auto& hack : hack::getUpdatedHacks())
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
    hack::initializeHacks();

    // Load keybinds
    keybinds::Manager::get()->init();

    gui::ThemeManager::get();

    // Add bitmap fonts to texture search path
    auto bmfontPath = geode::Mod::get()->getConfigDir() / "bmfonts";
    std::error_code ec;
    std::filesystem::create_directories(bmfontPath / GEODE_MOD_ID, ec);
    if (ec) {
        geode::log::warn("Failed to create bitmap fonts directory {}: {}", bmfontPath, ec.message());
    }

    utils::get<cocos2d::CCFileUtils>()->addSearchPath(geode::utils::string::pathToString(bmfontPath).c_str());
    geode::log::info("Added bitmap fonts to search path: {}", bmfontPath);

    // Add "Interface" tab to edit theme settings
    {
        using namespace gui;
        auto tab = MenuTab::find("tab.interface");
        std::vector<std::string> themeNames = {};
        for (auto const& theme : ThemeManager::listAvailableThemes()) {
            themeNames.push_back(theme.name);
        }
        if (!themeNames.empty()) {
            auto themeCombo = tab->addCombo("interface.theme", "themeIndex", themeNames, 0);
            themeCombo->callback([](int value) {
                ThemeManager::get()->loadTheme(ThemeManager::listAvailableThemes()[value].path);
                ThemeManager::get()->setUIScale(config::getTemp<float>("uiScale", 1.f));
            });
        }
        tab->addInputFloat("interface.ui-scale", "uiScale", 0.75f, 2.f, "x%.3f")
           ->callback([](float value) {
               ThemeManager::get()->setUIScale(value);
           })->disableSaving()->setFlags(ComponentFlags::DisableCocos);
        config::setIfEmpty("interface.dpi-scaling", true);
        tab->addToggle("interface.dpi-scaling")
           ->setDescription()
           ->setFlags(ComponentFlags::DisableCocos);

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

    #ifdef ECLIPSE_USE_FLOATING_BUTTON

        config::setIfEmpty<float>("float-btn.max-opacity", 1.f);
        config::setIfEmpty<float>("float-btn.min-opacity", 0.5f);
        config::setIfEmpty<bool>("float-btn.show-in-level", false);
        config::setIfEmpty<bool>("float-btn.show-in-editor", true);
        config::setIfEmpty<float>("float-btn.scale", 0.25f);
        tab->addLabel("float-btn.title")->setFlags(ComponentFlags::DisableImGui);
        tab->addInputFloat("float-btn.max-opacity", 0.1f, 1.f)->setFlags(ComponentFlags::DisableImGui);
        tab->addInputFloat("float-btn.min-opacity", 0.1f, 1.f)->setFlags(ComponentFlags::DisableImGui);
        tab->addToggle("float-btn.show-in-level")->setFlags(ComponentFlags::DisableImGui);
        tab->addToggle("float-btn.show-in-editor")->setFlags(ComponentFlags::DisableImGui);
        tab->addInputFloat("float-btn.scale", 0.15f, 1.f)->setFlags(ComponentFlags::DisableImGui);

    #endif

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
                            component->removeFlag(ComponentFlags::SearchedFor);
                    }

                    hasSearched = false;
                }
            } else {
                hasSearched = true;

                for (auto& tab : Engine::get()->getTabs()) {
                    bool hasFoundComponent = false;

                    for (auto& component : tab->getComponents()) {
                        if (utils::matchesStringFuzzy(i18n::get(component->getTitle()), input)) {
                            component->addFlag(ComponentFlags::SearchedFor);
                            hasFoundComponent = true;
                        } else component->removeFlag(ComponentFlags::SearchedFor);
                    }

                    tab->setSearchedFor(hasFoundComponent);
                }
            }
        })->disableSaving()->setFlags(ComponentFlags::DisableCocos | ComponentFlags::StartWithKeyboardFocus);
    }

    // Schedule hack updates
    cocos2d::CCScheduler::get()->scheduleSelector(
        schedule_selector(HackUpdater::update),
        HackUpdater::create(), 0.f, false
    );

    geode::listenForSettingChanges<std::string>("menu-style", [](std::string const& style) {
        gui::Engine::get()->setRenderer(
            style == "ImGui" ? gui::RendererType::ImGui : gui::RendererType::Cocos2d
        );
    });
}
