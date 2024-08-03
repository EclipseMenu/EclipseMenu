#include <modules/gui/gui.hpp>
#include <modules/hack/hack.hpp>
#include <modules/config/config.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

namespace eclipse::hacks::Global {

    void setPitch(float pitch) {
        static FMOD::DSP* pitchShifter = nullptr;
        
        pitch = config::get<bool>("global.pitchshift.toggle", false) ? pitch : 1.f;

        FMOD::System* system = FMODAudioEngine::sharedEngine()->m_system;

        if (pitchShifter) {
            FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->removeDSP(pitchShifter);
            pitchShifter->release();
            pitchShifter = nullptr;
        }
        
        if (pitch == 1.f)
            return;

        system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShifter);
        //pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 4096);
        pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 0x800); // or 0x457 
        pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
        FMODAudioEngine::sharedEngine()->m_backgroundMusicChannel->addDSP(0, pitchShifter);
    }

    class PitchShift : public hack::Hack {
        void init() override {
            auto tab = gui::MenuTab::find("Global");

            config::setIfEmpty("global.pitchshift.toggle", false);
            config::setIfEmpty("global.pitchshift", 1.f);

            tab->addFloatToggle("Pitch Shift", "global.pitchshift", 0.5f, 2.f, "%.2f")
                ->valueCallback(setPitch)
                ->handleKeybinds()
                ->toggleCallback([] {
                    if (config::get<bool>("global.pitchshift.toggle", false))
                        setPitch(config::get<float>("global.pitchshift"));
                    else
                        setPitch(1.f);
                });
        }

        void lateInit() override {
            if (config::get<bool>("global.pitchshift.toggle", false))
                setPitch(config::get<float>("global.pitchshift"));
        }

        [[nodiscard]] const char* getId() const override { return "Pitch Shift"; }
        [[nodiscard]] int32_t getPriority() const override { return -9; }
    };

    REGISTER_HACK(PitchShift)

}
