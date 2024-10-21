#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/PlayLayer.hpp>

namespace eclipse::hacks::Bypass {

    class InstantComplete : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");

            tab->addToggle("Instant Complete", "bypass.instantcomplete")->handleKeybinds();
        }

        [[nodiscard]] bool isCheating() override { return config::get<bool>("bypass.instantcomplete", false); }
        [[nodiscard]] const char* getId() const override { return "Instant Complete"; }
    };

    REGISTER_HACK(InstantComplete)

    class $modify(InstantCompletePLHook, PlayLayer) {
        ADD_HOOKS_DELEGATE("bypass.instantcomplete")

        void onEnterTransitionDidFinish() {
            PlayLayer::onEnterTransitionDidFinish();

            if (this->m_isPlatformer)
                this->playPlatformerEndAnimationToPos({ .0f, 105.f }, true);
            else
                this->playEndAnimationToPos({ 2.f, 2.f });
        }

        void levelComplete() {
            if (this->m_isPlatformer)
                this->m_timePlayed = 10.0;
            else
                this->m_attemptTime = 10.0;

            PlayLayer::levelComplete();
        }
    };
}
