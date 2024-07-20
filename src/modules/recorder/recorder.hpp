#pragma once

#ifndef GEODE_IS_ANDROID
#include "rendertexture.hpp"
#include "ffmpeg/ffmpegcli.hpp"

#include <mutex>

namespace eclipse::recorder {

    enum Codec {
        None = 0,
        h264_nvenc = 1,
        h264_amf = 2,
    };

    struct RenderSettings {
        uint32_t m_width = 1920;
        uint32_t m_height = 1080;
        uint32_t m_fps = 60;
        Codec m_codec = None;
        float m_bitrate = 30;
        std::string m_args;
        std::string m_extraArgs;
        std::string m_videoArgs;

        RenderSettings() : m_width(1920), m_height(1080), m_fps(60), m_bitrate(30) {}
        RenderSettings(uint32_t width, uint32_t height, uint32_t fps, float bitrate, Codec codec) : m_width(width), m_height(height), m_fps(fps), m_bitrate(bitrate), m_codec(codec) {}
    };

    class Recorder {
    public:
        Recorder(std::string ffmpegPath);

        void start(std::filesystem::path renderPath);
        void stop();

        void startAudio(std::filesystem::path renderPath);
        void stopAudio();

        void captureFrame();

        bool isRecording() { return m_recording; }
        bool isRecordingAudio() { return m_recordingAudio; }

    public:
        RenderSettings m_renderSettings;

    private:
        void recordThread();

    private:
        bool m_recording;
        bool m_recordingAudio;
        bool m_frameHasData;
        std::vector<uint8_t> m_currentFrame;
        std::mutex m_lock;
        RenderTexture m_renderTexture;
        std::filesystem::path m_renderPath;

        std::string m_ffmpegPath;

        ffmpegCLI* m_ffmpegCLI = nullptr;

    };
};
#endif
