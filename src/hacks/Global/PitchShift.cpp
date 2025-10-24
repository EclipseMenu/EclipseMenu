#include <modules/config/config.hpp>
#include <modules/gui/gui.hpp>
#include <modules/gui/components/float-toggle.hpp>
#include <modules/hack/hack.hpp>

#include <Geode/binding/FMODAudioEngine.hpp>

namespace eclipse::hacks::Global {
    void setPitch(float pitch) {
        static FMOD::DSP* pitchShifter = nullptr;

        pitch = config::get<bool>("global.pitchshift.toggle", false) ? pitch : 1.f;

        FMOD::System* system = utils::get<FMODAudioEngine>()->m_system;

        if (pitchShifter) {
            utils::get<FMODAudioEngine>()->m_backgroundMusicChannel->removeDSP(pitchShifter);
            pitchShifter->release();
            pitchShifter = nullptr;
        }

        if (pitch == 1.f)
            return;

        system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &pitchShifter);
        //pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 4096);
        pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 0x800); // or 0x457
        pitchShifter->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
        utils::get<FMODAudioEngine>()->m_backgroundMusicChannel->addDSP(0, pitchShifter);
    }

    class $hack(PitchShift) {
        void init() override {
            auto tab = gui::MenuTab::find("tab.global");

            config::setIfEmpty("global.pitchshift.toggle", false);
            config::setIfEmpty("global.pitchshift", 1.f);

            tab->addFloatToggle("global.pitchshift", 0.5f, 2.f, "%.2f")
               ->valueCallback(setPitch)
               ->handleKeybinds()
               ->setDescription()
               ->toggleCallback([] {
                   if (config::get<bool>("global.pitchshift.toggle", false))
                       setPitch(config::get<float>("global.pitchshift", 1.f));
                   else
                       setPitch(1.f);
               });
        }

        void lateInit() override {
            if (config::get<bool>("global.pitchshift.toggle", false))
                setPitch(config::get<float>("global.pitchshift", 1.f));
        }

        [[nodiscard]] const char* getId() const override { return "Pitch Shift"; }
        [[nodiscard]] int32_t getPriority() const override { return -9; }
    };

    REGISTER_HACK(PitchShift)
}
