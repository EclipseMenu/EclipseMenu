#include "recorder.hpp"

#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/general.hpp>
#include <thread>
#include <memory>
#include <utility>
#include <modules/utils/SingletonCache.hpp>
#include <modules/recorder/DSPRecorder.hpp>

namespace eclipse::recorder {
    namespace ffmpeg = ffmpeg::events;

    class ProjectionDelegate : public cocos2d::CCDirectorDelegate {
        virtual void updateProjection() override {
            kmGLMatrixMode(KM_GL_PROJECTION);
            kmGLLoadIdentity();
            kmMat4 orthoMatrix;
            auto size = utils::get<cocos2d::CCDirector>()->m_obWinSizeInPoints;
            kmMat4OrthographicProjection(&orthoMatrix, 0, size.width, size.height, 0, -1024, 1024 );
            kmGLMultMatrix(&orthoMatrix);
            kmGLMatrixMode(KM_GL_MODELVIEW);
            kmGLLoadIdentity();
        }
    };

    void Recorder::start() {
        m_currentFrame.resize(m_renderSettings.m_width * m_renderSettings.m_height * 4, 0);
        m_renderTexture.m_width = m_renderSettings.m_width;
        m_renderTexture.m_height = m_renderSettings.m_height;
        m_renderTexture.begin();

        m_recording = true;
        m_frameHasData = false;

        DSPRecorder::get()->start();

        utils::get<cocos2d::CCDirector>()->m_pProjectionDelegate = new ProjectionDelegate();
        std::thread(&Recorder::recordThread, this).detach();
    }

    void Recorder::stop() {
        m_recording = false;

        m_renderTexture.end();

        delete utils::get<cocos2d::CCDirector>()->m_pProjectionDelegate;
        utils::get<cocos2d::CCDirector>()->setProjection(cocos2d::ccDirectorProjection::kCCDirectorProjection2D);
    }

    void Recorder::captureFrame() {
        while (m_frameHasData) {}

        m_renderTexture.capture(m_lock, m_currentFrame, m_frameHasData);
    }

    void Recorder::recordThread() {
        geode::utils::thread::setName("Eclipse Recorder Thread");
        ffmpeg::Recorder ffmpegRecorder;
        if (!ffmpegRecorder.isValid()) {
            stop();
            m_callback("Failed to initialize ffmpeg recorder.");
            return;
        }

        auto res = ffmpegRecorder.init(m_renderSettings);
        if(res.isErr()) {
            stop();
            m_callback(res.unwrapErr());
            ffmpegRecorder.stop();
            return;
        }

        while (m_recording || m_frameHasData) {
            if (m_frameHasData) {
                m_lock.lock();
                m_frameHasData = false;

                res = ffmpegRecorder.writeFrame(m_currentFrame);
                if (res.isErr()) {
                    m_callback(res.unwrapErr());
                }

                m_lock.unlock();
            }
        }

        ffmpegRecorder.stop();

        DSPRecorder::get()->stop();
        auto data = DSPRecorder::get()->getData();

        std::filesystem::path tempPath = m_renderSettings.m_outputFile.parent_path() / "music.mp4";

        res = ffmpeg::AudioMixer::mixVideoRaw(m_renderSettings.m_outputFile, data, tempPath);
        if(res.isErr())
            m_callback(res.unwrapErr());

        std::filesystem::remove(m_renderSettings.m_outputFile);
        std::filesystem::rename(tempPath,m_renderSettings.m_outputFile);
    }

    std::vector<std::string> Recorder::getAvailableCodecs() {
        return ffmpeg::Recorder::getAvailableCodecs();
    }
}
