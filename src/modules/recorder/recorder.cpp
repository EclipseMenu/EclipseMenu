#include "recorder.hpp"

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
        ffmpeg::events::Recorder ffmpegRecorder;

        geode::Result<void> res = ffmpegRecorder.init(m_renderSettings);

        if(m_callback)
            m_callback(res);

        if(res.isErr()) {
            stop();
            ffmpegRecorder.stop();
            return;
        }

        while (m_recording || m_frameHasData) {
            if (m_frameHasData) {
                m_lock.lock();
                m_frameHasData = false;

                res = ffmpegRecorder.writeFrame(m_currentFrame);

                if(m_callback)
                    m_callback(res);
                    
                m_lock.unlock();
            }
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

        ffmpeg::events::AudioMixer audioMixer;
        audioMixer.mixVideoRaw(m_renderSettings.m_outputFile, data, tempPath);

        std::filesystem::remove(m_renderSettings.m_outputFile);
        std::filesystem::rename(tempPath,m_renderSettings.m_outputFile);
    }

    std::vector<std::string> Recorder::getAvailableCodecs() {
        ffmpeg::events::Recorder ffmpegRecorder;
        return ffmpegRecorder.getAvailableCodecs();
    }
}
