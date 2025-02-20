#include "recorder.hpp"

#include <memory>
#include <thread>
#include <utility>
#include <Geode/binding/FMODAudioEngine.hpp>
#include <Geode/loader/Log.hpp>
#include <Geode/utils/general.hpp>
#include <modules/debug/benchmark.hpp>
#include <modules/recorder/DSPRecorder.hpp>
#include <modules/utils/SingletonCache.hpp>
#include <utils.hpp>

namespace km {

// separated to not forget to update both platforms
#ifdef GEODE_IS_ARM_MAC
#define BINDINGS_VERSION 22074
#else
#define BINDINGS_VERSION 22074
#endif

#define DEFINE_ADDRESS(name, address, ...) \
    using func_t = decltype(&name); \
    auto func = reinterpret_cast<func_t>(geode::base::get() + address);\
    static_assert(GEODE_COMP_GD_VERSION == BINDINGS_VERSION, "Address for " #name " is outdated"); \
    return func(__VA_ARGS__)

    inline void GLMatrixMode(kmGLEnum mode) {
        #ifdef GEODE_IS_MACOS
        DEFINE_ADDRESS(kmGLMatrixMode, GEODE_ARM_MAC(0x1aba44) GEODE_INTEL_MAC(0x1f5f60), mode);
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
        DEFINE_ADDRESS(kmGLMultMatrix, GEODE_ARM_MAC(0x1abb60) GEODE_INTEL_MAC(0x1f6070), pIn);
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
        m_renderTexture = RenderTexture(m_renderSettings.m_width, m_renderSettings.m_height);
        m_renderTexture.begin();

        m_recording = true;

        DSPRecorder::get()->start();

        utils::get<cocos2d::CCDirector>()->m_pProjectionDelegate = new ProjectionDelegate();
        std::thread(&Recorder::recordThread, this).detach();
    }

    void Recorder::stop() {
        if (!m_recording) return;

        m_recording = false;

        // make sure to let the recording thread know that we're stopping
        m_frameReady.set(true);

        m_renderTexture.end();
        DSPRecorder::get()->stop();

        auto director = utils::get<cocos2d::CCDirector>();
        if (auto& delegate = utils::get<cocos2d::CCDirector>()->m_pProjectionDelegate) {
            delete delegate;
            delegate = nullptr;
        }

        director->setProjection(cocos2d::ccDirectorProjection::kCCDirectorProjection2D);
    }

    void Recorder::captureFrame() {
        // wait until the previous frame is processed
        m_frameReady.wait_for(false);

        // don't capture if we're not recording
        if (!m_recording) return;

        m_renderTexture.capture(utils::get<PlayLayer>(), m_currentFrame, m_frameReady);
    }

    std::string Recorder::getRecordingDuration() const {
        // m_recordingDuration is in nanoseconds
        double inSeconds = m_recordingDuration / 1'000'000'000.0;
        return utils::formatTime(inSeconds);
    }

    void Recorder::recordThread() {
        geode::utils::thread::setName("Eclipse Recorder Thread");
        geode::log::debug("Recorder thread started.");

        ffmpeg::Recorder ffmpegRecorder;
        if (!ffmpegRecorder.isValid()) {
            stop();
            m_callback("Failed to initialize ffmpeg recorder.");
            geode::log::debug("Recorder thread stopped.");
            return;
        }

        auto res = ffmpegRecorder.init(m_renderSettings);
        if (res.isErr()) {
            stop();
            m_callback(res.unwrapErr());
            ffmpegRecorder.stop();
            m_frameReady.set(false); // unlock the main thread if it's waiting
            geode::log::debug("Recorder thread stopped.");
            return;
        }

        // wait for the first frame
        m_frameReady.set(false);
        m_frameReady.wait_for(true);

        {
            // record the time it took to record the video (to show in the end)
            debug::Timer timer("Recording", &m_recordingDuration);

            while (m_recording) {
                res = ffmpegRecorder.writeFrame(m_currentFrame);
                if (res.isErr()) {
                    m_callback(res.unwrapErr());

                    // stop recording if an error occurred
                    this->stop();
                    break;
                }

                // break if we're not recording anymore (to avoid waiting forever)
                if (!m_recording) break;

                m_frameReady.set(false);
                m_frameReady.wait_for(true);
            }
        }

        geode::log::debug("Recorder thread stopped.");

        ffmpegRecorder.stop();

        DSPRecorder::get()->stop();
        auto data = DSPRecorder::get()->getData();

        std::filesystem::path tempPath = m_renderSettings.m_outputFile.parent_path() / "music.mp4";

        res = ffmpeg::AudioMixer::mixVideoRaw(m_renderSettings.m_outputFile, data, tempPath);
        if (res.isErr()) return m_callback(res.unwrapErr());

        std::error_code ec;
        std::filesystem::remove(m_renderSettings.m_outputFile, ec);
        if (ec) return m_callback("Failed to remove old video file.");

        ec = {};
        std::filesystem::rename(tempPath, m_renderSettings.m_outputFile, ec);
        if (ec) return m_callback("Failed to rename temporary video file.");
    }

    std::vector<std::string> Recorder::getAvailableCodecs() {
        return ffmpeg::Recorder::getAvailableCodecs();
    }
}
