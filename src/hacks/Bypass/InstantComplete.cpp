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

    class $modify(PlayLayer) {
        bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
            if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

            if (config::get<bool>("bypass.instantcomplete", false)) {
                if (this->m_isPlatformer)
                    this->playPlatformerEndAnimationToPos({ .0f, 105.f }, true);
                else
                    this->playEndAnimationToPos({ 2.f, 2.f });
            }

            return true;
        }

        void levelComplete() {
            if (config::get<bool>("bypass.instantcomplete", false)) {
                if (this->m_isPlatformer)
                    this->m_timePlayed = 10.0;
                else
                    this->m_attemptTime = 10.0;
            }

            PlayLayer::levelComplete();
        }
    };
}
