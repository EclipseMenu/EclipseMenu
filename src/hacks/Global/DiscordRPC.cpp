#ifdef GEODE_IS_DESKTOP
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/combo.hpp>
#include <modules/gui/components/input-float.hpp>
#include <modules/gui/components/input-text.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/i18n/translations.hpp>
#include <modules/labels/variables.hpp>

#include <discord_rpc.h>
#include <rift.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Global {
    std::chrono::time_point<std::chrono::steady_clock> s_lastDiscordUpdate;
    std::map<std::string, std::unique_ptr<rift::Script>> s_discordScripts;

    class $hack(DiscordRPC) {
        constexpr static auto DEFAULT_CLIENT_ID = "1212016614325624852";
        static time_t startTimestamp, levelTimestamp;

        static void handleDiscordError(int errcode, const char* message) {
            geode::log::error("Discord RPC error: {} ({})", message, errcode);
        }

        enum class GameState {
            Menu,
            Editor,
            Level,
            Platformer
        };

        static GameState getGameState() {
            if (auto* pl = utils::get<PlayLayer>())
                return pl->m_isPlatformer ? GameState::Platformer : GameState::Level;
            if (auto* ed = utils::get<LevelEditorLayer>())
                return GameState::Editor;
            return GameState::Menu;
        }

        void refreshPresence() {
            auto gameState = getGameState();
            auto getScript = [this, gameState](const std::string& key, bool addPrefix = true) -> rift::Script* {
                static auto nullScript = rift::compile("").unwrap(); // Script that returns empty string
                std::string keyStr;
                if (addPrefix) {
                    switch (gameState) {
                        case GameState::Menu: keyStr = "menu.";
                            break;
                        case GameState::Editor: keyStr = "editor.";
                            break;
                        case GameState::Level: keyStr = "level.";
                            break;
                        case GameState::Platformer: keyStr = "plat.";
                            break;
                    }
                }
                keyStr += key;
                auto it = s_discordScripts.find(keyStr);
                return it != s_discordScripts.end() ? it->second.get() : nullScript.get();
            };

            DiscordRichPresence presence = {};
            static std::string state, details, largeImage, largeImageText, smallImage, smallImageText, button1Text, button2Text, button1URL, button2URL;

            auto& varManager = labels::VariableManager::get();
            varManager.refetch(); // collect all variables
            const auto& vars = varManager.getVariables();
            state = getScript("state")->run(vars).unwrapOrDefault();
            details = getScript("details")->run(vars).unwrapOrDefault();
            largeImage = getScript("largeimage")->run(vars).unwrapOrDefault();
            largeImageText = getScript("largeimage.text")->run(vars).unwrapOrDefault();
            smallImage = getScript("smallimage")->run(vars).unwrapOrDefault();
            smallImageText = getScript("smallimage.text")->run(vars).unwrapOrDefault();
            button1Text = getScript("button1.text", false)->run(vars).unwrapOrDefault();
            button2Text = getScript("button2.text", false)->run(vars).unwrapOrDefault();
            button1URL = getScript("button1.url", false)->run(vars).unwrapOrDefault();
            button2URL = getScript("button2.url", false)->run(vars).unwrapOrDefault();

            presence.state = state.c_str();
            presence.details = details.c_str();
            presence.largeImageKey = largeImage.c_str();
            presence.largeImageText = largeImageText.c_str();
            presence.smallImageKey = smallImage.c_str();
            presence.smallImageText = smallImageText.c_str();
            presence.buttons[0].active = config::get<bool>("global.discordrpc.button1.enabled", false) && !button1URL.empty() && !button1Text.empty();
            presence.buttons[0].label = button1Text.c_str();
            presence.buttons[0].url = button1URL.c_str();
            presence.buttons[1].active = config::get<bool>("global.discordrpc.button2.enabled", false) && !button2URL.empty() && !button2Text.empty();
            presence.buttons[1].label = button2Text.c_str();
            presence.buttons[1].url = button2URL.c_str();

            // Time mode
            switch (config::get<int>("global.discordrpc.timemode", 1)) {
                default: break;
                case 1: { // Total playtime
                    presence.startTimestamp = startTimestamp;
                } break;
                case 2: if (gameState != GameState::Menu) { // Level playtime
                    presence.startTimestamp = levelTimestamp;
                } break;
                case 3: { // Total+Level playtime
                    presence.startTimestamp = gameState == GameState::Menu ? startTimestamp : levelTimestamp;
                } break;
            }

            Discord_UpdatePresence(&presence);
        }

        void toggleRPC(bool enabled) {
            if (enabled) refreshPresence();
            else Discord_ClearPresence();
        }

        static void initializeDiscord() {
            static bool initialized = false;
            if (initialized) {
                Discord_Shutdown();
                initialized = false;
            }

            DiscordEventHandlers handlers = {};
            handlers.errored = handleDiscordError;
            auto clientID = config::get<std::string>("global.discordrpc.clientid", DEFAULT_CLIENT_ID);
            Discord_Initialize(clientID.c_str(), &handlers, 1, nullptr);
            initialized = true;
        }

        void compileScript(const std::string& name) {
            auto res = rift::compile(config::get<std::string>("global.discordrpc." + name, ""));
            if (res.isOk()) {
                s_discordScripts[name] = std::move(res.unwrap());
            }
        }

        void recompileScripts() {
            // Delete all scripts
            s_discordScripts.clear();

            // Compile all scripts
            // Menu
            compileScript("menu.state");
            compileScript("menu.details");
            compileScript("menu.largeimage");
            compileScript("menu.largeimage.text");
            compileScript("menu.smallimage");
            compileScript("menu.smallimage.text");

            // Editor
            compileScript("editor.state");
            compileScript("editor.details");
            compileScript("editor.largeimage");
            compileScript("editor.largeimage.text");
            compileScript("editor.smallimage");
            compileScript("editor.smallimage.text");

            // Normal level
            compileScript("level.state");
            compileScript("level.details");
            compileScript("level.largeimage");
            compileScript("level.largeimage.text");
            compileScript("level.smallimage");
            compileScript("level.smallimage.text");

            // Platformer level
            compileScript("plat.state");
            compileScript("plat.details");
            compileScript("plat.largeimage");
            compileScript("plat.largeimage.text");
            compileScript("plat.smallimage");
            compileScript("plat.smallimage.text");

            // Buttons
            compileScript("button1.text");
            compileScript("button2.text");
            compileScript("button1.url");
            compileScript("button2.url");
        }

        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            config::setIfEmpty<std::string>("global.discordrpc.clientid", DEFAULT_CLIENT_ID);
            config::setIfEmpty("global.discordrpc.interval", 250.0f);
            config::setIfEmpty("global.discordrpc.timemode", 1);

            startTimestamp = std::time(nullptr);
            levelTimestamp = std::time(nullptr);

            // Setting default scripts:
            {
                // Menu
                config::setIfEmpty<std::string_view>("global.discordrpc.menu.details", "Browsing menus");
                config::setIfEmpty<std::string_view>("global.discordrpc.menu.state", "");
                config::setIfEmpty<std::string_view>("global.discordrpc.menu.largeimage", "circle");
                config::setIfEmpty<std::string_view>("global.discordrpc.menu.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty<std::string_view>("global.discordrpc.menu.smallimage", "");
                config::setIfEmpty<std::string_view>("global.discordrpc.menu.smallimage.text", "");

                // Editor
                config::setIfEmpty<std::string_view>("global.discordrpc.editor.details", "Working on \"{levelName}\"");
                config::setIfEmpty<std::string_view>("global.discordrpc.editor.state", "{objects} objects");
                config::setIfEmpty<std::string_view>("global.discordrpc.editor.largeimage", "circle");
                config::setIfEmpty<std::string_view>("global.discordrpc.editor.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty<std::string_view>("global.discordrpc.editor.smallimage", "editor");
                config::setIfEmpty<std::string_view>("global.discordrpc.editor.smallimage.text", "Editing a level");

                // Normal level
                config::setIfEmpty<std::string_view>("global.discordrpc.level.details", "{levelName} by {author}");
                config::setIfEmpty<std::string_view>("global.discordrpc.level.state", "Progress: {progress}% (Best {best}%)");
                config::setIfEmpty<std::string_view>("global.discordrpc.level.largeimage", "circle");
                config::setIfEmpty<std::string_view>("global.discordrpc.level.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty<std::string_view>("global.discordrpc.level.smallimage", "{difficultyKey}");
                config::setIfEmpty<std::string_view>("global.discordrpc.level.smallimage.text", "{levelStars}{starEmoji} (ID: {levelID})");

                // Platformer level
                config::setIfEmpty<std::string_view>("global.discordrpc.plat.details", "{levelName} by {author}");
                config::setIfEmpty<std::string_view>("global.discordrpc.plat.state", "Current time: {levelTime} s.");
                config::setIfEmpty<std::string_view>("global.discordrpc.plat.largeimage", "circle");
                config::setIfEmpty<std::string_view>("global.discordrpc.plat.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty<std::string_view>("global.discordrpc.plat.smallimage", "{difficultyKey}");
                config::setIfEmpty<std::string_view>("global.discordrpc.plat.smallimage.text", "{levelStars}{moonEmoji} (ID: {levelID})");

                // Buttons
                config::setIfEmpty("global.discordrpc.button1.enabled", false);
                config::setIfEmpty<std::string_view>("global.discordrpc.button1.text", "Button 1");
                config::setIfEmpty<std::string_view>("global.discordrpc.button1.url", "");
                config::setIfEmpty("global.discordrpc.button2.enabled", false);
                config::setIfEmpty<std::string_view>("global.discordrpc.button2.text", "Button 2");
                config::setIfEmpty<std::string_view>("global.discordrpc.button2.url", "");
            }

            initializeDiscord();
            recompileScripts();

            tab->addToggle("global.discordrpc")
               ->handleKeybinds()
               ->callback([this](bool enabled) { toggleRPC(enabled); })
               ->setDescription()
               ->addOptions([this](auto opt) {
                   opt->addInputText("global.discordrpc.client-id", "global.discordrpc.clientid");
                   opt->addInputFloat("global.discordrpc.update-rate", "global.discordrpc.interval", 100, FLT_MAX, "%.0f")
                      ->setDescription();
                   opt->addCombo(
                       "global.discordrpc.time-mode", "global.discordrpc.timemode",
                       {
                           i18n::get_("global.discordrpc.time-mode.0"),
                           i18n::get_("global.discordrpc.time-mode.1"),
                           i18n::get_("global.discordrpc.time-mode.2"),
                           i18n::get_("global.discordrpc.time-mode.3")
                       },
                       config::get<int>("global.discordrpc.timemode", 1)
                   )->setDescription();

                   #define ADD_SCRIPT(name, id) opt->addInputText(name, "global.discordrpc." id)->callback([this](auto){ compileScript(id); })

                   opt->addLabel("global.discordrpc.menus");
                   ADD_SCRIPT("global.discordrpc.menu.details", "menu.details");
                   ADD_SCRIPT("global.discordrpc.menu.state", "menu.state");
                   ADD_SCRIPT("global.discordrpc.menu.large-image", "menu.largeimage");
                   ADD_SCRIPT("global.discordrpc.menu.large-text", "menu.largeimage.text");
                   ADD_SCRIPT("global.discordrpc.menu.small-image", "menu.smallimage");
                   ADD_SCRIPT("global.discordrpc.menu.small-text", "menu.smallimage.text");

                   opt->addLabel("global.discordrpc.editor");
                   ADD_SCRIPT("global.discordrpc.editor.details", "editor.details");
                   ADD_SCRIPT("global.discordrpc.editor.state", "editor.state");
                   ADD_SCRIPT("global.discordrpc.editor.large-image", "editor.largeimage");
                   ADD_SCRIPT("global.discordrpc.editor.large-text", "editor.largeimage.text");
                   ADD_SCRIPT("global.discordrpc.editor.small-image", "editor.smallimage");
                   ADD_SCRIPT("global.discordrpc.editor.small-text", "editor.smallimage.text");

                   opt->addLabel("global.discordrpc.levels");
                   ADD_SCRIPT("global.discordrpc.level.details", "level.details");
                   ADD_SCRIPT("global.discordrpc.level.state", "level.state");
                   ADD_SCRIPT("global.discordrpc.level.large-image", "level.largeimage");
                   ADD_SCRIPT("global.discordrpc.level.large-text", "level.largeimage.text");
                   ADD_SCRIPT("global.discordrpc.level.small-image", "level.smallimage");
                   ADD_SCRIPT("global.discordrpc.level.small-text", "level.smallimage.text");

                   opt->addLabel("global.discordrpc.platformer");
                   ADD_SCRIPT("global.discordrpc.platformer.details", "plat.details");
                   ADD_SCRIPT("global.discordrpc.platformer.state", "plat.state");
                   ADD_SCRIPT("global.discordrpc.platformer.large-image", "plat.largeimage");
                   ADD_SCRIPT("global.discordrpc.platformer.large-text", "plat.largeimage.text");
                   ADD_SCRIPT("global.discordrpc.platformer.small-image", "plat.smallimage");
                   ADD_SCRIPT("global.discordrpc.platformer.small-text", "plat.smallimage.text");

                   opt->addLabel("global.discordrpc.buttons");
                   opt->addToggle("global.discordrpc.buttons.1", "global.discordrpc.button1.enabled")
                      ->addOptions([this](auto opt) {
                          ADD_SCRIPT("global.discordrpc.buttons.text", "button1.text");
                          ADD_SCRIPT("global.discordrpc.buttons.url", "button1.url");
                      });
                   opt->addToggle("global.discordrpc.buttons.2", "global.discordrpc.button2.enabled")
                      ->addOptions([this](auto opt) {
                          ADD_SCRIPT("global.discordrpc.buttons.text", "button2.text");
                          ADD_SCRIPT("global.discordrpc.buttons.url", "button2.url");
                      });
               });
        }

        void update() override {
            if (!config::get<bool>("global.discordrpc", false)) return;
            auto interval = config::get<float>("global.discordrpc.interval", 200.0f);
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - s_lastDiscordUpdate).count();
            if (static_cast<float>(elapsed) >= interval) {
                refreshPresence();
                s_lastDiscordUpdate = now;
            }
        }

        [[nodiscard]] const char* getId() const override { return "Discord RPC"; }
    };

    time_t DiscordRPC::startTimestamp = 0;
    time_t DiscordRPC::levelTimestamp = 0;

    REGISTER_HACK(DiscordRPC);

    class $modify(DiscordRPCGJBGLHook, GJBaseGameLayer) {
        bool init() override {
            if (!GJBaseGameLayer::init()) return false;
            DiscordRPC::levelTimestamp = std::time(nullptr);
            return true;
        }
    };
}
#endif
