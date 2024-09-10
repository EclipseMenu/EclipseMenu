#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameToolbox.hpp>

#ifndef GEODE_IS_WINDOWS // TODO: Make a patch for Windows
namespace eclipse::hacks::Level {

    class RandomSeed : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            config::setIfEmpty("level.randomseed.seed", 1);

            tab->addToggle("Random Seed", "level.randomseed")
                ->handleKeybinds()
                ->setDescription("Sets a seed of choice for all random triggers.")
                ->addOptions([](std::shared_ptr<gui::MenuTab> options) {
                    options->addInputInt("Seed", "level.randomseed.seed");
                });
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("level.randomseed", false); }
        [[nodiscard]] const char* getId() const override { return "Random Seed"; }
    };

    REGISTER_HACK(RandomSeed)

    class $modify(GameToolbox) {
        float fast_rand_0_1() {
            if (!config::get<bool>("level.randomseed", false))
                return GameToolbox::fast_rand_0_1();

            int newSeed = (214'013 * config::get<int>("level.randomseed.seed", false)) + 2'531'011;

            return ((newSeed >> 16) & 0x7FFF) / 32767.f;
        }
    };
}
#endif