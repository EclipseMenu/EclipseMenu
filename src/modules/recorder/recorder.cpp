#include "recorder.hpp"
#include <thread>

#ifndef GEODE_IS_ANDROID

#include <Geode/Geode.hpp>

#ifdef GEODE_IS_WINDOWS
#include "ffmpeg/windows/ffmpegWindows.hpp"
#endif

namespace eclipse::recorder {
    Recorder::Recorder(std::string ffmpegPath) {
        #ifdef GEODE_IS_WINDOWS
        m_ffmpegCLI = new ffmpegWindows();
        #endif

        m_ffmpegPath = ffmpegPath;
    }

    void Recorder::start(std::filesystem::path renderPath) {
        if (!m_ffmpegCLI)
            return;

        int result = std::system((m_ffmpegPath + " -version >nul 2>&1").c_str());

        if (result != 0)
            geode::log::error("FFmpeg not found {}", m_ffmpegPath);

        m_currentFrame.resize(m_renderSettings.m_width * m_renderSettings.m_height * 3, 0);
        m_renderTexture.m_width = m_renderSettings.m_width;
        m_renderTexture.m_height = m_renderSettings.m_height;
        m_renderTexture.begin();

        m_recording = true;
        m_frameHasData = false;

        m_renderPath = renderPath;

        std::thread t(&Recorder::recordThread, this);
        t.detach();
    }

    void Recorder::stop() {
        m_recording = false;

        m_renderTexture.end();
    }

    void Recorder::captureFrame() {
        while (m_frameHasData) {}

        m_renderTexture.capture(m_lock, m_currentFrame, m_frameHasData);
    }

    void Recorder::recordThread() {
        std::stringstream command;

        command << '"' << m_ffmpegPath << '"';
        command << " -y -f rawvideo -pix_fmt rgb24 -s " << m_renderSettings.m_width << "x" << m_renderSettings.m_height;
        command << " -r " << m_renderSettings.m_fps;

        if (!m_renderSettings.m_args.empty())
            command << " " << m_renderSettings.m_args;

        command << " -i - ";

        switch(m_renderSettings.m_codec) {
            case None:
                break;
            case h264_nvenc:
                command << "-c:v h264_nvenc ";
                break;
            case h264_amf:
                command << "-c:v h264_amf ";
                break;
        }

        command << "-b:v " << m_renderSettings.m_bitrate << "M ";

        if (!m_renderSettings.m_extraArgs.empty())
            command << m_renderSettings.m_extraArgs << " ";

        command << "-vf \"vflip";

        if (!m_renderSettings.m_videoArgs.empty())
            command << "," << m_renderSettings.m_videoArgs;

        command << "\" -an " << m_renderPath;

        geode::log::info("Recording to: {}", command.str());

        m_ffmpegCLI->open(command.str());

        while (m_recording || m_frameHasData) {
            m_lock.lock();

            if (m_frameHasData) {
                const geode::ByteVector frame = m_currentFrame;
                m_frameHasData = false;
                m_lock.unlock();
                m_ffmpegCLI->write(frame.data(), frame.size());
            }
            else m_lock.unlock();
        }

        m_ffmpegCLI->close();
    }

    void Recorder::startAudio(std::filesystem::path renderPath) {
        FMODAudioEngine::sharedEngine()->m_system->setOutput(FMOD_OUTPUTTYPE_WAVWRITER);
        m_recordingAudio = true;
    }

    void Recorder::stopAudio() {
        FMODAudioEngine::sharedEngine()->m_system->setOutput(FMOD_OUTPUTTYPE_AUTODETECT);
        m_recordingAudio = false;

        std::filesystem::path tempPath = m_renderPath.parent_path() / "music.mp4";

        std::stringstream command;

        command << m_ffmpegPath << " -y -i " << m_renderPath << " -i fmodoutput.wav -c:v copy -map 0:v -map 1:a "
           << tempPath;

        geode::log::info("Recording audio to: {}", command.str());

        m_ffmpegCLI->open(command.str());
        m_ffmpegCLI->close();

        std::filesystem::remove("fmodoutput.wav");

        std::filesystem::remove(m_renderPath);
        std::filesystem::rename(tempPath,m_renderPath);
    }
};
#endif
