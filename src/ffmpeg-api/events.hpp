#pragma once
#include "render_settings.hpp"

#include <Geode/loader/Event.hpp>

namespace ffmpeg::events {
namespace impl {
    constexpr size_t VTABLE_VERSION = 1;
    using CreateRecorder_t = void*(*)();
    using DeleteRecorder_t = void(*)(void*);
    using InitRecorder_t = geode::Result<>(*)(void*, const RenderSettings&);
    using StopRecorder_t = void(*)(void*);
    using WriteFrame_t = geode::Result<>(*)(void*, std::span<uint8_t const>);
    using GetAvailableCodecs_t = std::vector<std::string>(*)();
    using MixVideoAudio_t = geode::Result<>(*)(const std::filesystem::path&, const std::filesystem::path&, const std::filesystem::path&);
    using MixVideoRaw_t = geode::Result<>(*)(const std::filesystem::path&, std::span<float>, const std::filesystem::path&);

    struct VTable {
        CreateRecorder_t createRecorder = nullptr;
        DeleteRecorder_t deleteRecorder = nullptr;
        InitRecorder_t initRecorder = nullptr;
        StopRecorder_t stopRecorder = nullptr;
        WriteFrame_t writeFrame = nullptr;
        GetAvailableCodecs_t getAvailableCodecs = nullptr;
        MixVideoAudio_t mixVideoAudio = nullptr;
        MixVideoRaw_t mixVideoRaw = nullptr;
    };

    struct FetchVTableEvent : geode::Event<FetchVTableEvent, bool(VTable&, size_t)> {
        using Event::Event;
    };

    inline VTable& getVTable() {
        static VTable vtable;
        static bool initialized = false;
        if (!initialized) {
            initialized = FetchVTableEvent().send(vtable, VTABLE_VERSION);
        }
        return vtable;
    }
}

class Recorder {
public:
    Recorder() {
        auto& vtable = impl::getVTable();
        if (!vtable.createRecorder) {
            m_ptr = nullptr;
        } else {
            m_ptr = vtable.createRecorder();
        }
    }

    ~Recorder() {
        if (m_ptr) {
            auto& vtable = impl::getVTable();
            if (vtable.deleteRecorder) {
                vtable.deleteRecorder(m_ptr);
            }
        }
    }

    bool isValid() const { return m_ptr != nullptr; }

    /**
     * @brief Initializes the Recorder with the specified rendering settings.
     *
     * This function configures the recorder with the given render settings,
     * allocates necessary resources, and prepares for video encoding.
     *
     * @param settings The rendering settings that define the output characteristics,
     *                 including codec, bitrate, resolution, and pixel format.
     *
     * @return true if initialization is successful, false otherwise.
     */
    geode::Result<> init(RenderSettings const& settings) {
        auto& vtable = impl::getVTable();
        if (!vtable.initRecorder) {
            return geode::Err("FFmpeg API is not available.");
        }
        return vtable.initRecorder(m_ptr, settings);
    }
    /**
     * @brief Stops the recording process and finalizes the output file.
     *
     * This function ensures that all buffered frames are written to the output file,
     * releases allocated resources, and properly closes the output file.
     */
    void stop() {
        auto& vtable = impl::getVTable();
        if (vtable.stopRecorder) {
            vtable.stopRecorder(m_ptr);
        }
    }

    /**
     * @brief Writes a single video frame to the output.
     *
     * This function takes the frame data as a byte vector and encodes it
     * to the output file. The frame data must match the expected format and
     * dimensions defined during initialization.
     *
     * @param frameData A vector containing the raw frame data to be written.
     *
     * @return true if the frame is successfully written, false if there is an error.
     *
     * @warning Ensure that the frameData size matches the expected dimensions of the frame.
     */
    geode::Result<> writeFrame(std::span<uint8_t const> frameData) {
        auto& vtable = impl::getVTable();
        if (!vtable.writeFrame) {
            return geode::Err("FFmpeg API is not available.");
        }
        return vtable.writeFrame(m_ptr, frameData);
    }

    /**
     * @brief Retrieves a list of available codecs for video encoding.
     *
     * This function iterates through all available codecs in FFmpeg and
     * returns a sorted vector of codec names.
     *
     * @return A vector representing the names of available codecs.
     */
    static std::vector<std::string> getAvailableCodecs() {
        auto& vtable = impl::getVTable();
        if (!vtable.getAvailableCodecs) {
            return {};
        }
        return vtable.getAvailableCodecs();
    }

private:
    void* m_ptr = nullptr;
};

class AudioMixer {
public:
    AudioMixer() = delete;

    /**
     * @brief Mixes a video file and an audio file into a single MP4 output.
     *
     * This function takes an input video file and an audio file, and merges them into a single MP4 output file.
     * The output MP4 file will have both the video and audio streams synchronized.
     *
     * @param videoFile The path to the input video file.
     * @param audioFile The path to the input audio file.
     * @param outputMp4File The path where the output MP4 file will be saved.
     *
     * @warning The audio file is expected to contain stereo (dual-channel) audio. Using other formats might lead to unexpected results.
     * @warning The video file is expected to contain a single video stream. Only the first video stream will be copied.
     */
    static geode::Result<> mixVideoAudio(std::filesystem::path const& videoFile, std::filesystem::path const& audioFile, std::filesystem::path const& outputMp4File) {
        auto& vtable = impl::getVTable();
        if (!vtable.mixVideoAudio) {
            return geode::Err("FFmpeg API is not available.");
        }
        return vtable.mixVideoAudio(videoFile, audioFile, outputMp4File);
    }

    /**
     * @brief Mixes a video file and raw audio data into a single MP4 output.
     *
     * This function takes an input video file and raw audio data (in the form of a vector of floating-point samples),
     * and merges them into a single MP4 output file.
     *
     * @param videoFile The path to the input video file.
     * @param raw A vector containing the raw audio data (floating-point samples).
     * @param outputMp4File The path where the output MP4 file will be saved.
     *
     * @warning The raw audio data is expected to be stereo (dual-channel). Using mono or multi-channel audio might lead to issues.
     * @warning The video file is expected to contain a single video stream. Only the first video stream will be copied.
     */
    static geode::Result<> mixVideoRaw(std::filesystem::path const& videoFile, std::span<float> raw, std::filesystem::path const& outputMp4File) {
        auto& vtable = impl::getVTable();
        if (!vtable.mixVideoRaw) {
            return geode::Err("FFmpeg API is not available.");
        }
        return vtable.mixVideoRaw(videoFile, raw, outputMp4File);
    }
};

}