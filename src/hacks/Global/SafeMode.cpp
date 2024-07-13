#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Global {

    class AutoSafeMode : public hack::Hack {
    public:
        static bool shouldEnable() {
            if (!config::get<bool>("global.autosafemode", false))
                return false;

            for (auto& hack : hack::Hack::getHacks()) {
                if (hack->isCheating())
                    return true;
            }

            return false;
        }

    private:
        void init() override {
            auto tab = gui::MenuTab::find("Global");
            config::setIfEmpty("global.autosafemode", true);
            tab->addToggle("Auto Safe Mode", "global.autosafemode")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Auto Safe Mode"; }
    };

    class SafeMode : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");
            tab->addToggle("Safe Mode", "global.safemode")->handleKeybinds();
        }

        [[nodiscard]] const char* getId() const override { return "Safe Mode"; }
    };

    REGISTER_HACK(AutoSafeMode)
    REGISTER_HACK(SafeMode)

    class $modify(PlayLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("PlayLayer::levelComplete");
            SAFE_PRIORITY("PlayLayer::destroyPlayer");
        }

        void levelComplete() {
            bool original = m_isTestMode;
            bool safeMode = config::get<bool>("global.safemode", false);
            if (safeMode || AutoSafeMode::shouldEnable()) {
                m_isTestMode = true;
            }
            PlayLayer::levelComplete();
            m_isTestMode = original;
        }

        void destroyPlayer(PlayerObject* player, GameObject* object) override {
            bool original = m_isTestMode;
            bool safeMode = config::get<bool>("global.safemode", false);
            if (safeMode || AutoSafeMode::shouldEnable()) {
                m_isTestMode = true;
            }
            PlayLayer::destroyPlayer(player, object);
            m_isTestMode = original;
        }
    };

}
