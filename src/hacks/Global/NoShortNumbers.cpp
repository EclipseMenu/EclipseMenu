#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/GameToolbox.hpp>

namespace eclipse::hacks::Global {
    class NoShortNumbers : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");
            tab->addToggle("global.noshortnumbers")->setDescription()->handleKeybinds()
               ->addOptions([](auto options) {
                   options->addToggle("global.noshortnumbers.thousands");
               });
        }

        [[nodiscard]] const char* getId() const override { return "No Short Numbers"; }
    };

    REGISTER_HACK(NoShortNumbers)

    class $modify(NoShortNumbersGTHook, GameToolbox) {
        ALL_DELEGATES_AND_SAFE_PRIO("global.noshortnumbers")

        static gd::string intToShortString(int value) {
            bool thousands = config::get<bool>("global.noshortnumbers.thousands", false);
            gd::string str = thousands
                                 ? fmt::format("{}", fmt::group_digits(value))
                                 : fmt::format("{}", value);
            return str;
        }
    };
}
