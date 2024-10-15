#include "DSPRecorder.hpp"

DSPRecorder* DSPRecorder::get() {
    static DSPRecorder instance;
    static bool initialized = false;
    if (!initialized) {
        instance.init();
        initialized = true;
    }
    return &instance;
}

void DSPRecorder::start() {
    if (m_recording) return;

    m_masterGroup->addDSP(0, m_dsp);
    std::lock_guard lock(m_lock);
    m_data.clear();
    m_recording = true;
}

void DSPRecorder::stop() {
    if (!m_recording) return;
    if (m_useLocking) unlock();

    m_masterGroup->removeDSP(m_dsp);
    std::lock_guard lock(m_lock);
    m_recording = false;
}

std::vector<float> DSPRecorder::getData() {
    std::lock_guard lock(m_lock);
    auto data = m_data;
    m_data.clear();
    return data;
}

void DSPRecorder::unlock() {
    // m_canContinue = true;
    // m_canContinue.notify_one();
}

void DSPRecorder::init() {
    FMOD_DSP_DESCRIPTION desc = {};
    strcpy(desc.name, "DSP Recorder");
    desc.numinputbuffers = 1;
    desc.numoutputbuffers = 1;
    desc.read = [](FMOD_DSP_STATE*, float* inbuffer, float*, unsigned int length, int, int* outchannels) {
        auto recorder = DSPRecorder::get();
        if (!recorder->m_recording) return FMOD_OK;

        {
            std::lock_guard lock(recorder->m_lock);
            auto channels = *outchannels;
            recorder->m_data.insert(recorder->m_data.end(), inbuffer, inbuffer + length * channels);
        }

        // if (recorder->m_useLocking) {
        //     recorder->m_canContinue.wait(false);
        //     recorder->m_canContinue = false;
        // }

        return FMOD_OK;
    };

    auto system = FMODAudioEngine::sharedEngine()->m_system;
    system->createDSP(&desc, &m_dsp);
    system->getMasterChannelGroup(&m_masterGroup);
}
