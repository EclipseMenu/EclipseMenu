#pragma once
#include <Geode/platform/platform.hpp>
#include "ffmpeg-api/events.hpp"

#include "rendertexture.hpp"

#include <mutex>

namespace eclipse::recorder {
    class Recorder {
    public:
        void start();
        void stop();

        void captureFrame();

        bool isRecording() const { return m_recording; }
        std::string getRecordingDuration() const;

        void setCallback(const std::function<void(std::string const&)>& callback) { m_callback = callback; }

        static std::vector<std::string> getAvailableCodecs();

    public:
        ffmpeg::RenderSettings m_renderSettings{};

    private:
        void recordThread();

    private:
        bool m_recording = false;
        bool m_frameHasData = false;
        std::vector<uint8_t> m_currentFrame;
        std::mutex m_lock;
        std::condition_variable m_cv;
        RenderTexture m_renderTexture{};
        uint64_t m_recordingDuration = 0;

        std::function<void(std::string const&)> m_callback;
    };
};
