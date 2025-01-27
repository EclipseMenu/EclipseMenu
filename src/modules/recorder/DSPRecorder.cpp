#include "DSPRecorder.hpp"
#include <modules/config/config.hpp>
#include <modules/utils/SingletonCache.hpp>
#include <modules/config/config.hpp>

#include "ffmpeg-api/events.hpp"

DSPRecorder* DSPRecorder::get() {
    static DSPRecorder instance;
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        instance.init();
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

    m_masterGroup->removeDSP(m_dsp);
    std::lock_guard lock(m_lock);
    m_recording = false;

    m_masterGroup->setPaused(false);
}

std::vector<float> DSPRecorder::getData() {
    std::lock_guard lock(m_lock);
    auto data = m_data;
    m_data.clear();
    return data;
}

void DSPRecorder::init() {
    auto system = eclipse::utils::get<FMODAudioEngine>()->m_system;

    FMOD_DSP_DESCRIPTION desc = {};
    strcpy(desc.name, "DSP Recorder");
    desc.numinputbuffers = 1;
    desc.numoutputbuffers = 1;
    desc.read = [](FMOD_DSP_STATE*, float* inbuffer, float* outbuffer, unsigned int length, int, int* outchannels) {
        auto recorder = DSPRecorder::get();
        if (!recorder->m_recording) return FMOD_OK;

        auto channels = *outchannels;

        {
            std::lock_guard lock(recorder->m_lock);
            recorder->m_data.insert(recorder->m_data.end(), inbuffer, inbuffer + length * channels);
        }

        std::memcpy(outbuffer, inbuffer, length * channels * sizeof(float));

        FMOD::ChannelGroup* master;
        eclipse::utils::get<FMODAudioEngine>()->m_system->getMasterChannelGroup(&master);
        master->setPaused(true);

        return FMOD_OK;
    };

    system->createDSP(&desc, &m_dsp);
    system->getMasterChannelGroup(&m_masterGroup);
}

void DSPRecorder::tryUnpause(float time) const {
    auto system = eclipse::utils::get<FMODAudioEngine>()->m_system;
    int sampleRate;
    int channels;
    system->getSoftwareFormat(&sampleRate, nullptr, &channels);

    float songTime = (float) m_data.size() / ((float) sampleRate * (float) channels);

    if (time >= songTime) m_masterGroup->setPaused(false);
}
