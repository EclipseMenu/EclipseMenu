#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>
#include <modules/labels/variables.hpp>

#include <Geode/modify/GameManager.hpp>

namespace eclipse::hacks::Global {
    class $hack(CBFToggle) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            tab->addToggle("global.clickbetweensteps")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                   utils::get<GameManager>()->setGameVariable(GameVar::ClickBetweenSteps, v);
                   if (auto gl = utils::get<GJBaseGameLayer>()) {
                       gl->m_clickBetweenSteps = v;
                   }
               })
               ->disableSaving();

            tab->addToggle("global.clickonsteps")
               ->handleKeybinds()
               ->setDescription()
               ->callback([](bool v) {
                   utils::get<GameManager>()->setGameVariable(GameVar::ClickOnSteps, v);
                   if (auto gl = utils::get<GJBaseGameLayer>()) {
                       gl->m_clickOnSteps = v;
                   }
               })
               ->disableSaving();
        }

        void lateInit() override {
            auto GM = utils::get<GameManager>();
            config::setTemp("global.clickbetweensteps", GM->getGameVariable(GameVar::ClickBetweenSteps));
            config::setTemp("global.clickonsteps", GM->getGameVariable(GameVar::ClickOnSteps));

            auto cbf = geode::Loader::get()->getLoadedMod("syzzi.click_between_frames");
            if (!cbf) return; // mod not loaded

            auto tab = gui::MenuTab::find("tab.global");
            tab->addToggle("global.click-between-frames", "syzzi.click_between_frames.toggle")
               ->handleKeybinds()
               ->setDescription()
               ->callback([cbf](bool v) {
                   // soft-toggle means disable if true, so we invert the value
                   cbf->setSettingValue<bool>("soft-toggle", !v);
               })->disableSaving();

            listenForSettingChanges<bool>("soft-toggle", [](bool v) {
                config::setTemp("syzzi.click_between_frames.toggle", !v);
                labels::VariableManager::get().setVariable("cbf", !v);
            }, cbf);

            bool isCBF = !cbf->getSettingValue<bool>("soft-toggle");
            config::setTemp("syzzi.click_between_frames.toggle", isCBF);
            labels::VariableManager::get().setVariable("cbf", isCBF);
        }

        [[nodiscard]] const char* getId() const override { return "Click Between Frames"; }
    };

    REGISTER_HACK(CBFToggle)

    class $modify(CBSCOSGMHook, GameManager) {
        void setGameVariable(char const* key, bool value) {
            GameManager::setGameVariable(key, value);
            if (strcmp(key, GameVar::ClickBetweenSteps) == 0) {
                config::setTemp("global.clickbetweensteps", value);
            } else if (strcmp(key, GameVar::ClickOnSteps) == 0) {
                config::setTemp("global.clickonsteps", value);
            }
        }
    };
}
