#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameToolbox.hpp>

namespace eclipse::hacks::Global {

    class NoShortNumbers : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            tab->addToggle("No Short Numbers", "global.noshortnumbers")
                ->setDescription("Removes any abbreviation. (Example: 23.4K -> 23400)")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "No Short Numbers"; }
    };

    REGISTER_HACK(NoShortNumbers)

    class $modify(NoShortNumbersGTHook, GameToolbox) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("GameToolbox::intToShortString");
        }

        static gd::string intToShortString(int value) {
            if (!config::get<bool>("global.noshortnumbers", false))
                return GameToolbox::intToShortString(value);

            gd::string str = fmt::format("{}", value);
            return str;
        }
    };

}
