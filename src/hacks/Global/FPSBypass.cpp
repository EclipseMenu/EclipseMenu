#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/float-toggle.hpp>
#include <modules/hack/hack.hpp>

#ifdef GEODE_IS_DESKTOP
constexpr float MIN_FPS = 10.f;
constexpr float MAX_FPS = 100000.f;

#ifdef GEODE_IS_WINDOWS
#include <Geode/modify/GameManager.hpp>
#endif

namespace eclipse::hacks::Global {
    class $hack(FPSBypass) {
    public:
        #ifdef GEODE_IS_WINDOWS
        static void updateRefreshRate() {
            auto* gm = utils::get<GameManager>();

            // store settings
            bool fpsBypassEnabled = config::get<bool>("global.fpsbypass.toggle", false);
            auto fpsBypassValue = config::get<float>("global.fpsbypass", gm->m_customFPSTarget);
            float actualFPS = std::clamp(fpsBypassValue, MIN_FPS, MAX_FPS); // sometimes the value can be 0
            gm->setGameVariable(GameVar::UnlockFPS, fpsBypassEnabled);
            gm->m_customFPSTarget = actualFPS;

            // apply settings
            float frameTime = 1.f / (fpsBypassEnabled ? actualFPS : 60.f);
            utils::get<cocos2d::CCDirector>()->setAnimationInterval(frameTime);
        }
        #else
        static void updateRefreshRate() {
            static auto sdl = geode::Loader::get()->getInstalledMod("zmx.sdl");
            if (!sdl || !sdl->isLoaded()) return;

            bool fpsBypassEnabled = config::get<bool>("global.fpsbypass.toggle", false);
            auto fpsBypassValue = config::get<float>("global.fpsbypass", static_cast<float>(sdl->getSettingValue<int64_t>("framerate-limit")));
            int actualFPS = static_cast<int>(std::clamp(fpsBypassValue, MIN_FPS, MAX_FPS)); // sometimes the value can be 0
            sdl->setSettingValue("uncap-framerate", fpsBypassEnabled);
            sdl->setSettingValue("framerate-limit", actualFPS);
        }
        #endif

        void init() override {
            #ifdef GEODE_IS_MACOS
            auto sdl = geode::Loader::get()->getInstalledMod("zmx.sdl");
            if (!sdl || !sdl->shouldLoad()) return;
            #endif

            auto tab = gui::MenuTab::find("tab.global");
            tab->addFloatToggle("global.fpsbypass", "global.fpsbypass", MIN_FPS, MAX_FPS, "%.2f FPS")
               ->handleKeybinds()
               ->toggleCallback([GEODE_MACOS(sdl)] {
                   if (config::get<bool>("global.fpsbypass.toggle", false)) {
                       config::setTemp("global.vsync", false);
                       #ifdef GEODE_IS_WINDOWS
                       utils::get<GameManager>()->setGameVariable(GameVar::VerticalSync, false);
                       utils::get<AppDelegate>()->toggleVerticalSync(false);
                       #else
                       if (sdl->isLoaded()) {
                           sdl->setSettingValue("disable-vsync", true);
                       }
                       #endif
                   }
                   updateRefreshRate();
               })
               ->valueCallback([](float) { updateRefreshRate(); });
        }

        void lateInit() override {
            #ifdef GEODE_IS_WINDOWS
            auto* gm = utils::get<GameManager>();
            auto fpsBypassEnabled = gm->getGameVariable(GameVar::UnlockFPS);
            auto fpsBypassValue = gm->m_customFPSTarget;
            if (fpsBypassValue == 0) // rare robtop bug
                fpsBypassValue = 60.f;
            config::set("global.fpsbypass", fpsBypassValue);
            config::set("global.fpsbypass.toggle", fpsBypassEnabled);
            #else
            auto sdl = geode::Loader::get()->getInstalledMod("zmx.sdl");
            if (!sdl || !sdl->isLoaded()) return;

            auto fpsBypassEnabled = sdl->getSettingValue<bool>("uncap-framerate");
            auto fpsBypassValue = sdl->getSettingValue<int64_t>("framerate-limit");

            config::set("global.fpsbypass", static_cast<float>(fpsBypassValue));
            config::set("global.fpsbypass.toggle", fpsBypassEnabled);

            geode::listenForSettingChanges("uncap-framerate", [](bool enabled) {
                config::set("global.fpsbypass.toggle", enabled);
                if (enabled) {
                    config::setTemp("global.vsync", false);
                    sdl->setSettingValue("disable-vsync", true);
                }
            }, sdl);
            geode::listenForSettingChanges("framerate-limit", [](int64_t value) {
                config::set("global.fpsbypass", static_cast<float>(value));
            }, sdl);
            #endif

            updateRefreshRate();
        }

        [[nodiscard]] const char* getId() const override { return "FPSBypass"; }
        [[nodiscard]] int32_t getPriority() const override { return -15; }
    };

    REGISTER_HACK(FPSBypass)

    #ifdef GEODE_IS_WINDOWS
    class $modify(FPSBypassGMHook, GameManager) {
        void setGameVariable(char const* key, bool value) {
            GameManager::setGameVariable(key, value);
            if (strcmp(key, GameVar::UnlockFPS) == 0 && value) {
                if (this->getGameVariable(GameVar::VerticalSync)) {
                    config::setTemp("global.vsync", false);
                    GameManager::setGameVariable(GameVar::VerticalSync, false);
                    // extra safety if called before AppDelegate is ready
                    geode::queueInMainThread([] {
                        utils::get<AppDelegate>()->toggleVerticalSync(false);
                    });
                }
            }
        }
    };
    #endif
}
#endif
