#pragma once
#include <Geode/platform/platform.hpp>
#include "ffmpeg-api/events.hpp"

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

        void setCallback(const std::function<void(geode::Result<>)>& callback) { m_callback = callback; }

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

        std::function<void(geode::Result<>)> m_callback;
    };
};
