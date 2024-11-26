#ifdef GEODE_IS_DESKTOP
#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>
#include <modules/labels/variables.hpp>

#include <discord_rpc.h>
#include <rift.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Global {

    class DiscordRPC : public hack::Hack {
    public:
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
            if (auto* pl = PlayLayer::get())
                return pl->m_isPlatformer ? GameState::Platformer : GameState::Level;
            if (auto* ed = LevelEditorLayer::get())
                return GameState::Editor;
            return GameState::Menu;
        }

        void refreshPresence() {
            auto gameState = getGameState();
            auto getScript = [this, gameState](const std::string& key, bool addPrefix = true) -> rift::Script* {
                static rift::Script* nullScript = rift::compile("").getValue(); // Script that returns empty string
                std::string keyStr;
                if (addPrefix) {
                    switch (gameState) {
                        case GameState::Menu: keyStr = "menu."; break;
                        case GameState::Editor: keyStr = "editor."; break;
                        case GameState::Level: keyStr = "level."; break;
                        case GameState::Platformer: keyStr = "plat."; break;
                    }
                }
                keyStr += key;
                auto it = m_scripts.find(keyStr);
                return it != m_scripts.end() ? it->second.get() : nullScript;
            };

            DiscordRichPresence presence = {};
            static std::string state, details, largeImage, largeImageText, smallImage, smallImageText, button1Text, button2Text, button1URL, button2URL;

            auto& varManager = labels::VariableManager::get();
            varManager.refetch(); // collect all variables
            const auto& vars = varManager.getVariables();
            state = getScript("state")->run(vars);
            details = getScript("details")->run(vars);
            largeImage = getScript("largeimage")->run(vars);
            largeImageText = getScript("largeimage.text")->run(vars);
            smallImage = getScript("smallimage")->run(vars);
            smallImageText = getScript("smallimage.text")->run(vars);
            button1Text = getScript("button1.text", false)->run(vars);
            button2Text = getScript("button2.text", false)->run(vars);
            button1URL = getScript("button1.url", false)->run(vars);
            button2URL = getScript("button2.url", false)->run(vars);

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
            if (res)
                m_scripts[name] = std::unique_ptr<rift::Script>(res.getValue());
        }

        void recompileScripts() {
            // Delete all scripts
            m_scripts.clear();

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
            auto tab = gui::MenuTab::find("Global");

            config::setIfEmpty<std::string>("global.discordrpc.clientid", DEFAULT_CLIENT_ID);
            config::setIfEmpty("global.discordrpc.interval", 250.0f);
            config::setIfEmpty("global.discordrpc.timemode", 1);

            startTimestamp = std::time(nullptr);
            levelTimestamp = std::time(nullptr);

            // Setting default scripts:
            {
                // Menu
                config::setIfEmpty("global.discordrpc.menu.details", "Browsing menus");
                config::setIfEmpty("global.discordrpc.menu.state", "");
                config::setIfEmpty("global.discordrpc.menu.largeimage", "circle");
                config::setIfEmpty("global.discordrpc.menu.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty("global.discordrpc.menu.smallimage", "");
                config::setIfEmpty("global.discordrpc.menu.smallimage.text", "");

                // Editor
                config::setIfEmpty("global.discordrpc.editor.details", "Working on \"{levelName}\"");
                config::setIfEmpty("global.discordrpc.editor.state", "{objects} objects");
                config::setIfEmpty("global.discordrpc.editor.largeimage", "circle");
                config::setIfEmpty("global.discordrpc.editor.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty("global.discordrpc.editor.smallimage", "editor");
                config::setIfEmpty("global.discordrpc.editor.smallimage.text", "Editing a level");

                // Normal level
                config::setIfEmpty("global.discordrpc.level.details", "{levelName} by {author}");
                config::setIfEmpty("global.discordrpc.level.state", "Progress: {progress}% (Best {best}%)");
                config::setIfEmpty("global.discordrpc.level.largeimage", "circle");
                config::setIfEmpty("global.discordrpc.level.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty("global.discordrpc.level.smallimage", "{difficultyKey}");
                config::setIfEmpty("global.discordrpc.level.smallimage.text", "{levelStars}{starEmoji} (ID: {levelID})");

                // Platformer level
                config::setIfEmpty("global.discordrpc.plat.details", "{levelName} by {author}");
                config::setIfEmpty("global.discordrpc.plat.state", "Current time: {levelTime} s.");
                config::setIfEmpty("global.discordrpc.plat.largeimage", "circle");
                config::setIfEmpty("global.discordrpc.plat.largeimage.text", "Geometry Dash ({username})");
                config::setIfEmpty("global.discordrpc.plat.smallimage", "{difficultyKey}");
                config::setIfEmpty("global.discordrpc.plat.smallimage.text", "{levelStars}{moonEmoji} (ID: {levelID})");

                // Buttons
                config::setIfEmpty("global.discordrpc.button1.enabled", false);
                config::setIfEmpty("global.discordrpc.button1.text", "Button 1");
                config::setIfEmpty("global.discordrpc.button1.url", "");
                config::setIfEmpty("global.discordrpc.button2.enabled", false);
                config::setIfEmpty("global.discordrpc.button2.text", "Button 2");
                config::setIfEmpty("global.discordrpc.button2.url", "");
            }

            initializeDiscord();
            recompileScripts();

            tab->addToggle("Discord RPC", "global.discordrpc")
                ->handleKeybinds()
                ->callback([this](bool enabled) { toggleRPC(enabled); })
                ->setDescription("Display your current status in Discord.")
                ->addOptions([this](auto opt) {
                    opt->addInputText("Client ID", "global.discordrpc.clientid");
                    opt->addInputFloat("Update interval (ms)", "global.discordrpc.interval", 100, FLT_MAX, "%.0f")
                        ->setDescription("How often the RPC should update. Lower values may cause lag.");
                    opt->addCombo("Time mode", "global.discordrpc.timemode",
                        {"Disabled", "Total playtime", "Level playtime", "Total+Level playtime"},
                        config::get<int>("global.discordrpc.timemode", 1)
                    )->setDescription(
                        "Defines how the time is displayed in the RPC.\n"
                        "Disabled: Time is not displayed.\n"
                        "Total playtime: Displays total time you've been in the game.\n"
                        "Level playtime: Only displays the time you've been in the current level.\n"
                        "Total+Level playtime: Displays total time in menus and level time in levels."
                    );

#define ADD_SCRIPT(name, id) opt->addInputText(name, "global.discordrpc." id)->callback([this](auto){ recompileScripts(); })

                    opt->addLabel("Menus");
                    ADD_SCRIPT("Menu Details", "menu.details");
                    ADD_SCRIPT("Menu State", "menu.state");
                    ADD_SCRIPT("Menu Large Image", "menu.largeimage");
                    ADD_SCRIPT("Menu Large Image Text", "menu.largeimage.text");
                    ADD_SCRIPT("Menu Small Image", "menu.smallimage");
                    ADD_SCRIPT("Menu Small Image Text", "menu.smallimage.text");

                    opt->addLabel("Editor");
                    ADD_SCRIPT("Editor Details", "editor.details");
                    ADD_SCRIPT("Editor State", "editor.state");
                    ADD_SCRIPT("Editor Large Image", "editor.largeimage");
                    ADD_SCRIPT("Editor Large Image Text", "editor.largeimage.text");
                    ADD_SCRIPT("Editor Small Image", "editor.smallimage");
                    ADD_SCRIPT("Editor Small Image Text", "editor.smallimage.text");

                    opt->addLabel("Levels");
                    ADD_SCRIPT("Level Details", "level.details");
                    ADD_SCRIPT("Level State", "level.state");
                    ADD_SCRIPT("Level Large Image", "level.largeimage");
                    ADD_SCRIPT("Level Large Image Text", "level.largeimage.text");
                    ADD_SCRIPT("Level Small Image", "level.smallimage");
                    ADD_SCRIPT("Level Small Image Text", "level.smallimage.text");

                    opt->addLabel("Platformer");
                    ADD_SCRIPT("Platformer Details", "plat.details");
                    ADD_SCRIPT("Platformer State", "plat.state");
                    ADD_SCRIPT("Platformer Large Image", "plat.largeimage");
                    ADD_SCRIPT("Platformer Large Image Text", "plat.largeimage.text");
                    ADD_SCRIPT("Platformer Small Image", "plat.smallimage");
                    ADD_SCRIPT("Platformer Small Image Text", "plat.smallimage.text");

                    opt->addLabel("Buttons");
                    opt->addToggle("Button 1", "global.discordrpc.button1.enabled")
                        ->addOptions([this](auto opt){
                            ADD_SCRIPT("Text", "button1.text");
                            ADD_SCRIPT("URL", "button1.url");
                        });
                    opt->addToggle("Button 2", "global.discordrpc.button2.enabled")
                        ->addOptions([this](auto opt){
                            ADD_SCRIPT("Text", "button2.text");
                            ADD_SCRIPT("URL", "button2.url");
                        });
                });
        }

        void update() override {
            if (!config::get<bool>("global.discordrpc", false)) return;
            auto interval = config::get<float>("global.discordrpc.interval", 200.0f);
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastUpdate).count();
            if (static_cast<float>(elapsed) >= interval) {
                refreshPresence();
                m_lastUpdate = now;
            }
        }

        [[nodiscard]] const char* getId() const override { return "Discord RPC"; }

        std::chrono::time_point<std::chrono::steady_clock> m_lastUpdate;
        std::map<std::string, std::unique_ptr<rift::Script>> m_scripts;
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