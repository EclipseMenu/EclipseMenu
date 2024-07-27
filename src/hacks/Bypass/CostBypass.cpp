#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameStatsManager.hpp>

namespace eclipse::hacks::Bypass {

    class CostBypass : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Cost Bypass", "bypass.costbypass")
                ->handleKeybinds()
                ->setDescription("Sets the cost of all shop items to 0 orbs. (requires restart)");
        }

        [[nodiscard]] const char* getId() const override { return "Cost Bypass"; }
    };

    REGISTER_HACK(CostBypass)

    class $modify(GameStatsManager) {
        void addStoreItem(int p0, int p1, int p2, int p3, ShopType p4) {
            if (config::get<bool>("bypass.costbypass", false)) {
                GameStatsManager::addStoreItem(p0, p1, p2, 0, p4);
            } else {
                GameStatsManager::addStoreItem(p0, p1, p2, p3, p4);
            };
        }
    };
}
