#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/binding/GameManager.hpp>

namespace eclipse::hacks::Level {

    void onChangeShowCoins(bool state) {
        static cocos2d::CCObject* uncollectedSecretCoin = nullptr;
        static cocos2d::CCObject* uncollectedUserCoin = nullptr;

        auto* GM = GameManager::sharedState();

        if (!uncollectedSecretCoin)
            uncollectedSecretCoin = GM->m_unkAnimationDict->objectForKey(-142)->copy();

        if (!uncollectedUserCoin)
            uncollectedUserCoin = GM->m_unkAnimationDict->objectForKey(-1329)->copy();

        if (state) {
            uncollectedSecretCoin->retain();
            uncollectedUserCoin->retain();

            GM->m_unkAnimationDict->removeObjectForKey(-142);
            GM->m_unkAnimationDict->removeObjectForKey(-1329);

            GM->m_unkAnimationDict->setObject(GM->m_unkAnimationDict->objectForKey(142)->copy(), -142);
            GM->m_unkAnimationDict->setObject(GM->m_unkAnimationDict->objectForKey(1329)->copy(), -1329);
        } else if (uncollectedSecretCoin && uncollectedUserCoin) {
            GM->m_unkAnimationDict->removeObjectForKey(-142);
            GM->m_unkAnimationDict->removeObjectForKey(-1329);

            GM->m_unkAnimationDict->setObject(uncollectedSecretCoin, -142);
            GM->m_unkAnimationDict->setObject(uncollectedUserCoin, -1329);
        }
    }

    class AlwaysShowCoins : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Always Show Coins", "level.alwaysshowcoins")
                ->handleKeybinds()
                ->setDescription("Always shows the uncollected variant of the coins.")
                ->callback(onChangeShowCoins);
        }

        void lateInit() override {
            onChangeShowCoins(config::get<bool>("level.alwaysshowcoins", false));
        }

        [[nodiscard]] const char* getId() const override { return "Always Show Coins"; }
    };

    REGISTER_HACK(AlwaysShowCoins)

    // if qolmod makes 3 billion playlayers this will not work correctly
    static bool skipUniqueCoin = false;

    class $modify(AlwaysShowCoinsGOHook, GameObject) {
        void playDestroyObjectAnim(GJBaseGameLayer* p0) {
            if (config::get<bool>("level.alwaysshowcoins", false))
                skipUniqueCoin = true;

            GameObject::playDestroyObjectAnim(p0);

            skipUniqueCoin = false;
        }
    };

    class $modify(AlwaysShowCoinsBGLHook, GJBaseGameLayer) {
        bool hasUniqueCoin(EffectGameObject* p0) {
            if (!GJBaseGameLayer::hasUniqueCoin(p0))
                return false;

            return !skipUniqueCoin;
        }
    };
}
