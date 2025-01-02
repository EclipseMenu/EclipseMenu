#include "recorder.hpp"

#include <memory>
#include <thread>
#include <utility>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/general.hpp>
#include <modules/recorder/DSPRecorder.hpp>
#include <modules/utils/SingletonCache.hpp>

namespace km {

// separated to not forget to update both platforms
#ifdef GEODE_IS_ARM_MAC
#define BINDINGS_VERSION 22074
#else
#define BINDINGS_VERSION 22074
#endif

#define DEFINE_ADDRESS(name, address, ...) \
    using func_t = decltype(&name); \
    constexpr func_t func = (func_t)address;\
    static_assert(GEODE_COMP_GD_VERSION == BINDINGS_VERSION, "Address for " ##name " is outdated"); \
    return func(__VA_ARGS__)

    inline void GLMatrixMode(kmGLEnum mode) {
        #ifdef GEODE_IS_MACOS
        DEFINE_ADDRESS(kmGLMatrixMode, GEODE_ARM_MAC(0x1aba44) GEODE_INTEL_MAC(0x1a9f44), mode);
        #else
        kmGLMatrixMode(mode);
        #endif
    }

    inline void GLLoadIdentity() {
        #ifdef GEODE_IS_MACOS
        DEFINE_ADDRESS(kmGLLoadIdentity, GEODE_ARM_MAC(0x1abafc) GEODE_INTEL_MAC(0x1f6010));
        #else
        kmGLLoadIdentity();
        #endif
    }

    inline kmMat4* Mat4OrthographicProjection(kmMat4* pOut, float left, float right, float bottom, float top, float nearVal, float farVal) {
        #ifdef GEODE_IS_MACOS
        DEFINE_ADDRESS(kmMat4OrthographicProjection, GEODE_ARM_MAC(0x34a914) GEODE_INTEL_MAC(0x3c1240), pOut, left, right, bottom, top, nearVal, farVal);
        #else
        return kmMat4OrthographicProjection(pOut, left, right, bottom, top, nearVal, farVal);
        #endif
    }

    inline void GLMultMatrix(const kmMat4* pIn) {
        #ifdef GEODE_IS_MACOS
        DEFINE_ADDRESS(kmGLMultMatrix, GEODE_ARM_MAC(0x1abb60) GEODE_INTEL_MAC(0x1f6070));
        #else
        kmGLMultMatrix(pIn);
        #endif
    }
}

namespace eclipse::recorder {
    namespace ffmpeg = ffmpeg::events;

    class ProjectionDelegate : public cocos2d::CCDirectorDelegate {
        void updateProjection() override {
            km::GLMatrixMode(KM_GL_PROJECTION);
            km::GLLoadIdentity();
            kmMat4 orthoMatrix;
            auto size = utils::get<cocos2d::CCDirector>()->m_obWinSizeInPoints;
            km::Mat4OrthographicProjection(&orthoMatrix, 0, size.width, size.height, 0, -1024, 1024);
            km::GLMultMatrix(&orthoMatrix);
            km::GLMatrixMode(KM_GL_MODELVIEW);
            km::GLLoadIdentity();
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
        if (res.isErr()) {
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
        if (res.isErr()) m_callback(res.unwrapErr());

        std::filesystem::remove(m_renderSettings.m_outputFile);
        std::filesystem::rename(tempPath, m_renderSettings.m_outputFile);
    }

    std::vector<std::string> Recorder::getAvailableCodecs() {
        return ffmpeg::Recorder::getAvailableCodecs();
    }
}
