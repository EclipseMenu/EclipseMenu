#pragma once
#include <vector>
#include <Geode/fmod/fmod.hpp>

class DSPRecorder {
public:
    static DSPRecorder* get();

    /// @brief Enable the DSP and start recording PCM data.
    void start();
    /// @brief Disable the DSP and stop recording PCM data.
    void stop();

    /// @brief Returns the recorded PCM data and clears the buffer.
    std::vector<float> getData();

    [[nodiscard]] bool isRecording() const { return m_recording; }

    void tryUnpause(float time);

private:
    void init();

    FMOD::DSP* m_dsp = nullptr;
    FMOD::ChannelGroup* m_masterGroup = nullptr;
    std::vector<float> m_data;
    std::mutex m_lock;

    bool m_recording = false;
};