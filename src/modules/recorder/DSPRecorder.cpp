#include "DSPRecorder.hpp"
#include <modules/utils/SingletonCache.hpp>
#include <modules/config/config.hpp>

bool recreating = false;

FMOD_RESULT setSoftwareFormat(FMOD::System* system, int samplerate, FMOD_SPEAKERMODE speakermode, int numrawspeakers) {
    float tps = eclipse::config::get<bool>("global.tpsbypass.toggle", false) ? eclipse::config::get<float>("global.tpsbypass", 240.f) : 240.f;
    if(recreating)
        samplerate *= tps / eclipse::config::get<float>("recorder.fps", 60.f);
    return system->setSoftwareFormat(samplerate, speakermode, numrawspeakers);
}

$execute {
    auto address = geode::addresser::getNonVirtual(&FMOD::System::setSoftwareFormat);
    auto result = geode::Mod::get()->hook(reinterpret_cast<void *>(address), &setSoftwareFormat, "setSoftwareFormat");
    if (result.isErr())
        geode::log::error("Failed to hook setSoftwareFormat");
}

DSPRecorder* DSPRecorder::get() {
    static DSPRecorder instance;
    return &instance;
}

void DSPRecorder::start() {
    if (m_recording) return;

    init();

    m_masterGroup->addDSP(0, m_dsp);
    std::lock_guard lock(m_lock);
    m_data.clear();
    m_recording = true;
}

void DSPRecorder::stop() {
    if (!m_recording) return;

    auto engine = eclipse::utils::get<FMODAudioEngine>();
    engine->~FMODAudioEngine();
    new (engine) FMODAudioEngine();
    engine->setupAudioEngine();

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

void DSPRecorder::init() {
    recreating = true;
    auto engine = eclipse::utils::get<FMODAudioEngine>();
    engine->~FMODAudioEngine();
    new (engine) FMODAudioEngine();
    engine->setupAudioEngine();
    recreating = false;
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
        FMODAudioEngine::get()->m_system->getMasterChannelGroup(&master);
        master->setPaused(true);

        return FMOD_OK;
    };

    float tps = eclipse::config::get<bool>("global.tpsbypass.toggle", false) ? eclipse::config::get<float>("global.tpsbypass", 240.f) : 240.f;

    system->createDSP(&desc, &m_dsp);
    system->getMasterChannelGroup(&m_masterGroup);
    m_masterGroup->setPitch(tps / eclipse::config::get<float>("recorder.fps", 60.f) * 1.5f);
}

void DSPRecorder::tryUnpause(float time) {
    auto system = eclipse::utils::get<FMODAudioEngine>()->m_system;
    int sampleRate;
    int channels;
    system->getSoftwareFormat(&sampleRate, nullptr, &channels);

    float fps = eclipse::config::get<float>("recorder.fps", 60.f);
    float tps = eclipse::config::get<bool>("global.tpsbypass.toggle", false) ? eclipse::config::get<float>("global.tpsbypass", 240.f) : 240.f;
    float mult = tps / fps * 1.5f;

    float songTime = (float)m_data.size() / ((float)sampleRate * (float)channels) * mult;

    if(time >= songTime)
        m_masterGroup->setPaused(false);
}