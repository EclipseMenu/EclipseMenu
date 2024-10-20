#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameToolbox.hpp>

namespace eclipse::hacks::Global {

    class NoShortNumbers : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            tab->addToggle("No Short Numbers", "global.noshortnumbers")
                ->setDescription("Removes any number abbreviations. (Example: 23.4K -> 23400)")
                ->handleKeybinds()
                ->addOptions([](auto options) {
                    options->addToggle("Thousands Separator", "global.noshortnumbers.thousands");
                });
        }

        [[nodiscard]] const char* getId() const override { return "No Short Numbers"; }
    };

    REGISTER_HACK(NoShortNumbers)

    class $modify(NoShortNumbersGTHook, GameToolbox) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.noshortnumbers")

        static gd::string intToShortString(int value) {
            bool thousands = config::get<bool>("global.noshortnumbers.thousands", false);
            gd::string str = thousands ? fmt::format("{}", fmt::group_digits(value))
                                       : fmt::format("{}", value);
            return str;
        }
    };

}
