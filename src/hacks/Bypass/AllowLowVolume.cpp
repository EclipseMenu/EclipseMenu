#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>

namespace eclipse::hacks::Bypass {

    class AllowLowVolume : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Bypass");
            tab->addToggle("Allow Low Volume", "bypass.allowlowvolume")
                ->handleKeybinds()
                ->setDescription("Allows setting the volume lower than 3%.");
        }

        [[nodiscard]] const char* getId() const override { return "Allow Low Volume"; }
    };

    REGISTER_HACK(AllowLowVolume)

#define GET_SLIDER(sender) geode::cast::typeinfo_cast<SliderThumb*>(sender); if (!slider) return

    class $modify(OptionsLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("OptionsLayer::musicSliderChanged");
            SAFE_PRIORITY("OptionsLayer::sfxSliderChanged");
        }

        void musicSliderChanged(cocos2d::CCObject* sender) {
            if (!config::get<bool>("bypass.allowlowvolume", false))
                return OptionsLayer::musicSliderChanged(sender);

            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            auto* audioEngine = FMODAudioEngine::get();
            float originalVolume = audioEngine->getBackgroundMusicVolume();
            audioEngine->setBackgroundMusicVolume(value);
            if (originalVolume <= 0.f && value > 0.f)
                GameManager::get()->playMenuMusic();
        }

        void sfxSliderChanged(cocos2d::CCObject* sender) {
            if (!config::get<bool>("bypass.allowlowvolume", false))
                return OptionsLayer::sfxSliderChanged(sender);

            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            FMODAudioEngine::get()->setEffectsVolume(value);
        }
    };

    class $modify(PauseLayer) {
        static void onModify(auto& self) {
            SAFE_PRIORITY("PauseLayer::musicSliderChanged");
            SAFE_PRIORITY("PauseLayer::sfxSliderChanged");
        }

        void musicSliderChanged(cocos2d::CCObject* sender) {
            if (!config::get<bool>("bypass.allowlowvolume", false))
                return PauseLayer::musicSliderChanged(sender);

            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            FMODAudioEngine::get()->setBackgroundMusicVolume(value);
        }

// Function is merged with the one in OptionsLayer on Windows
#if !(defined(GEODE_IS_WINDOWS) && GEODE_COMP_GD_VERSION == 22060)
        void sfxSliderChanged(cocos2d::CCObject* sender) {
            if (!config::get<bool>("bypass.allowlowvolume", false))
                return PauseLayer::sfxSliderChanged(sender);

            auto slider = GET_SLIDER(sender);
            auto value = slider->getValue();
            FMODAudioEngine::get()->setEffectsVolume(value);
        }
#endif
    };
}
