#include "recorder.hpp"

#include <eclipse.ffmpeg-api/include/recorder.hpp>
#include <eclipse.ffmpeg-api/include/audio_mixer.hpp>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/general.hpp>
#include <thread>
#include <memory>
#include <utility>

#include <modules/recorder/DSPRecorder.hpp>

namespace eclipse::recorder {

    void Recorder::start() {
        m_currentFrame.resize(m_renderSettings.m_width * m_renderSettings.m_height * 4, 0);
        m_renderTexture.m_width = m_renderSettings.m_width;
        m_renderTexture.m_height = m_renderSettings.m_height;
        m_renderTexture.begin();

        m_recording = true;
        m_frameHasData = false;

        std::thread(&Recorder::recordThread, this).detach();
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
        ffmpeg::Recorder ffmpegRecorder;

        ffmpegRecorder.init(m_renderSettings);

        while (m_recording || m_frameHasData) {
            m_lock.lock();

            if (m_frameHasData) {
                m_frameHasData = false;
                ffmpegRecorder.writeFrame(m_currentFrame);
                m_lock.unlock();
            }
            else m_lock.unlock();
        }

        ffmpegRecorder.stop();
    }

    void Recorder::startAudio(const std::filesystem::path& renderPath) {
        DSPRecorder::get()->useLocking(false);
        DSPRecorder::get()->start();
        m_recordingAudio = true;
    }

    void Recorder::stopAudio() {
        DSPRecorder::get()->stop();
        auto data = DSPRecorder::get()->getData();
        m_recordingAudio = false;

        std::filesystem::path tempPath = m_renderSettings.m_outputFile.parent_path() / "music.mp4";

        ffmpeg::AudioMixer audioMixer;
        audioMixer.mixVideoRaw(m_renderSettings.m_outputFile, data, tempPath);

        std::filesystem::remove(m_renderSettings.m_outputFile);
        std::filesystem::rename(tempPath,m_renderSettings.m_outputFile);
    }

    std::vector<std::string> Recorder::getAvailableCodecs() {
        ffmpeg::Recorder ffmpegRecorder;
        return ffmpegRecorder.getAvailableCodecs();
    }
}
