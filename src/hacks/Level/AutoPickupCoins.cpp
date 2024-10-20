#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Level {

    class AutoPickupCoins : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Auto Pickup Coins", "level.autopickupcoins")
                ->handleKeybinds()
                ->setDescription("Automatically picks up coins");
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("level.autopickupcoins", false); }
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
