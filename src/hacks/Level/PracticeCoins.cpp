#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/GJBaseGameLayer.hpp>

namespace eclipse::hacks::Level {

    class PracticeCoins : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Level");

            tab->addToggle("Practice Coins", "level.practicecoins")
                ->setDescription("Allows you to collect coins in practice mode.")
                ->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Practice Coins"; }
    };

    REGISTER_HACK(PracticeCoins)

    class $modify(PracticeCoinsBGLHook, GJBaseGameLayer) {
        void collisionCheckObjects(PlayerObject* player, gd::vector<GameObject*>* gameObjects, int p2, float p3) {
            if (!config::get<bool>("level.practicecoins", false))
                return GJBaseGameLayer::collisionCheckObjects(player, gameObjects, p2, p3);

            bool isPractice = m_isPracticeMode;
            m_isPracticeMode = false;
            GJBaseGameLayer::collisionCheckObjects(player, gameObjects, p2, p3);
            m_isPracticeMode = isPractice;
        }
    };
}