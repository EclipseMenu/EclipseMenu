#ifndef GEODE_IS_MACOS
// TODO: someone look at fixing this for imac
#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/binding/GameManager.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {
    void onChangeShowCoins(bool state) {
        static cocos2d::CCObject* uncollectedSecretCoin = nullptr;
        static cocos2d::CCObject* uncollectedUserCoin = nullptr;

        auto* GM = utils::get<GameManager>();

        if (!uncollectedSecretCoin)
            uncollectedSecretCoin = GM->m_mainFramesForAnimation->objectForKey(-142)->copy();

        if (!uncollectedUserCoin)
            uncollectedUserCoin = GM->m_mainFramesForAnimation->objectForKey(-1329)->copy();

        if (state) {
            uncollectedSecretCoin->retain();
            uncollectedUserCoin->retain();

            GM->m_mainFramesForAnimation->removeObjectForKey(-142);
            GM->m_mainFramesForAnimation->removeObjectForKey(-1329);

            GM->m_mainFramesForAnimation->setObject(GM->m_mainFramesForAnimation->objectForKey(142)->copy(), -142);
            GM->m_mainFramesForAnimation->setObject(GM->m_mainFramesForAnimation->objectForKey(1329)->copy(), -1329);
        } else if (uncollectedSecretCoin && uncollectedUserCoin) {
            GM->m_mainFramesForAnimation->removeObjectForKey(-142);
            GM->m_mainFramesForAnimation->removeObjectForKey(-1329);

            GM->m_mainFramesForAnimation->setObject(uncollectedSecretCoin, -142);
            GM->m_mainFramesForAnimation->setObject(uncollectedUserCoin, -1329);
        }
    }

    class $hack(AlwaysShowCoins) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.level");
            tab->addToggle("level.alwaysshowcoins")->handleKeybinds()->setDescription()
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
        ADD_HOOKS_DELEGATE("level.alwaysshowcoins")

        void playDestroyObjectAnim(GJBaseGameLayer* p0) {
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
#endif