#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {
    class $hack(AutoPickupCoins) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.autopickupcoins")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { RETURN_CACHED_BOOL("level.autopickupcoins"); }
        [[nodiscard]] const char* getId() const override { return "Auto Pickup Coins"; }
    };

    REGISTER_HACK(AutoPickupCoins)

    class $modify(AutoPickupCoinsPLHook, PlayLayer) {
        struct Fields {
            std::vector<EffectGameObject*> m_coins;
        };

        static void onModify(auto& self) {
            HOOKS_TOGGLE("level.autopickupcoins", PlayLayer, "resetLevel");
        }

        void addObject(GameObject* obj) {
            PlayLayer::addObject(obj);
            auto id = obj->m_objectID;

            if (id == 142 || id == 1329)
                m_fields->m_coins.push_back(static_cast<EffectGameObject*>(obj));
        }

        void resetLevel() {
            PlayLayer::resetLevel();

            for (auto* coin : m_fields->m_coins) {
                if (coin == nullptr) continue;
                this->pickupItem(coin);
                this->destroyObject(coin);
            }
        }
    };
}
