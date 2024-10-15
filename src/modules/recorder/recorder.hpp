#pragma once
#include <Geode/platform/platform.hpp>
#include <eclipse.ffmpeg-api/include/render_settings.hpp>

#include "rendertexture.hpp"

#include <mutex>
#include <chrono>

namespace eclipse::recorder {

    class Recorder {
    public:
        void start();
        void stop();

        void startAudio(const std::filesystem::path& renderPath);
        void stopAudio();

        void captureFrame();

        bool isRecording() const { return m_recording; }
        bool isRecordingAudio() const { return m_recordingAudio; }

        std::vector<std::string> getAvailableCodecs();
    
    public:
        ffmpeg::RenderSettings m_renderSettings{};

    private:
        void recordThread();

    private:
        bool m_recording;
        bool m_recordingAudio;
        bool m_frameHasData;
        std::vector<uint8_t> m_currentFrame;
        std::mutex m_lock;
        RenderTexture m_renderTexture;
    };
};
