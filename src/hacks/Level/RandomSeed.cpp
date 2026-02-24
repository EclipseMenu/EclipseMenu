#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(RandomSeed) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");

            config::setIfEmpty("level.randomseed.seed", 1);

            tab->addToggle("level.randomseed")
               ->handleKeybinds()
               ->setDescription()
               ->addOptions([](auto options) {
                   options->addToggle("level.randomseed.constantseed")->setDescription();
                   options->addInputInt("level.randomseed.seed");
               });
        }

        [[nodiscard]] bool isCheating() const override { return config::get<"level.randomseed", bool>(); }
        [[nodiscard]] char const* getId() const override { return "Random Seed"; }
    };

    REGISTER_HACK(RandomSeed)

    class $modify(RandomSeedGJBGLHook, GJBaseGameLayer) {
        inline static geode::Hook* s_hook = nullptr;
        static void handleToggle() {
            bool value = config::get("level.randomseed", false);
            bool constantSeed = config::get("level.randomseed.constantseed", false);
            if (s_hook) {
                (void) s_hook->toggle(value && constantSeed);
            }
        }

        static void onModify(auto& self) {
            SAFE_HOOKS_ALL();

            #ifndef GEODE_IS_MACOS
            s_hook = self.getHook("GJBaseGameLayer::processCommands").unwrapOrDefault();
            #else
            s_hook = self.getHook("GJBaseGameLayer::processQueuedButtons").unwrapOrDefault();
            #endif

            if (!s_hook) return;

            auto value = config::get("level.randomseed", false);
            s_hook->setAutoEnable(value);

            config::addDelegate("level.randomseed", handleToggle, true);
            config::addDelegate("level.randomseed.constantseed", handleToggle, true);
        }

        #ifndef GEODE_IS_MACOS
        void processCommands(float dt, bool isHalfTick, bool isLastTick) {
            GameToolbox::fast_srand(config::get<"level.randomseed.seed", int>(1));
            GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        }
        #else
        void processQueuedButtons(float dt, bool clearInputQueue) {
            GameToolbox::fast_srand(config::get<"level.randomseed.seed", int>(1));
            GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);
        }
        #endif
    };

    class $modify(RandomSeedPLHook, PlayLayer) {
        inline static geode::Hook* s_hook = nullptr;
        static void handleToggle() {
            bool value = config::get("level.randomseed", false);
            bool constantSeed = config::get("level.randomseed.constantseed", false);
            if (s_hook) {
                (void) s_hook->toggle(value && !constantSeed);
            }
        }

        static void onModify(auto& self) {
            SAFE_HOOKS_ALL();

            s_hook = self.getHook("PlayLayer::resetLevel").unwrapOrDefault();
            if (!s_hook) return;

            auto value = config::get("level.randomseed", false);
            s_hook->setAutoEnable(value);

            config::addDelegate("level.randomseed", handleToggle, true);
            config::addDelegate("level.randomseed.constantseed", handleToggle, true);
        }

        void resetLevel() {
            PlayLayer::resetLevel();
            GameToolbox::fast_srand(config::get<"level.randomseed.seed", int>(1));
        }
    };
}
