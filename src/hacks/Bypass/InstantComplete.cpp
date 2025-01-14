#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Bypass {
    class $hack(InstantComplete) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.bypass");
            tab->addToggle("bypass.instantcomplete")->handleKeybinds()->setDescription();
        }

        [[nodiscard]] bool isCheating() const override { RETURN_CACHED_BOOL("bypass.instantcomplete"); }
        [[nodiscard]] const char* getId() const override { return "Instant Complete"; }
    };

    REGISTER_HACK(InstantComplete)

    class $modify(InstantCompletePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("bypass.instantcomplete")

        void onEnterTransitionDidFinish() {
            PlayLayer::onEnterTransitionDidFinish();

            if (this->m_isPlatformer)
                this->playPlatformerEndAnimationToPos({.0f, 105.f}, true);
            else this->playEndAnimationToPos({2.f, 2.f});
        }

        void levelComplete() {
            if (this->m_isPlatformer) this->m_timePlayed = 10.0;
            else this->m_attemptTime = 10.0;

            PlayLayer::levelComplete();
        }
    };
}
