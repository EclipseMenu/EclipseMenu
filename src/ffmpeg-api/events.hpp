#pragma once

#include "render_settings.hpp"

#include <Geode/loader/Event.hpp>

namespace ffmpeg::events {

class CreateRecorderEvent : public geode::Event {
public:
    void setPtr(void* ptr) {m_ptr = ptr;}
    void* getPtr() {return m_ptr;}
private:
    void* m_ptr;
};

class DeleteRecorderEvent : public geode::Event {
public:
    DeleteRecorderEvent(void* ptr) {m_ptr = ptr;}
    void* getPtr() {return m_ptr;}
private:
    void* m_ptr;
};

class InitRecorderEvent : public geode::Event {
public:
    InitRecorderEvent(void* ptr, RenderSettings settings) {
        m_ptr = ptr;
        m_renderSettings = settings;
    }

    void setResult(const geode::Result<void>& result) {m_result = geode::Result(result);}
    geode::Result<void> getResult() {return m_result;}

    void* getPtr() {return m_ptr;}

    const RenderSettings& getRenderSettings() {return m_renderSettings;}

private:
    RenderSettings m_renderSettings;
    void* m_ptr;
    geode::Result<void> m_result = geode::Ok();
};

class StopRecorderEvent : public geode::Event {
public:
    StopRecorderEvent(void* ptr) {m_ptr = ptr;}
    void* getPtr() {return m_ptr;}
private:
    void* m_ptr;
};

class WriteFrameRecorderEvent : public geode::Event {
public:
    WriteFrameRecorderEvent(void* ptr, const std::vector<uint8_t>& frameData) {
        m_ptr = ptr;
        m_frameData = &frameData;
    }

    void setResult(const geode::Result<void>& result) {m_result = geode::Result(result);}
    geode::Result<void> getResult() {return m_result;}

    void* getPtr() {return m_ptr;}

    const std::vector<uint8_t>& getFrameData() {return *m_frameData;}

private:
    const std::vector<uint8_t>* m_frameData;
    void* m_ptr;
    geode::Result<void> m_result = geode::Ok();
};

class CodecRecorderEvent : public geode::Event {
public:
    CodecRecorderEvent(void* ptr) {m_ptr = ptr;}
    void* getPtr() {return m_ptr;}

    void setCodecs(std::vector<std::string> codecs) {m_codecs = codecs;}
    std::vector<std::string> getCodecs() {return m_codecs;}
private:
    void* m_ptr;
    std::vector<std::string> m_codecs;
};

class CreateMixerEvent : public geode::Event {
public:
    void setPtr(void* ptr) {m_ptr = ptr;}
    void* getPtr() {return m_ptr;}
private:
    void* m_ptr;
};

class DeleteMixerEvent : public geode::Event {
public:
    DeleteMixerEvent(void* ptr) {m_ptr = ptr;}
    void* getPtr() {return m_ptr;}
private:
    void* m_ptr;
};

class MixVideoAudioEvent : public geode::Event {
public:
    MixVideoAudioEvent(void* ptr, std::filesystem::path videoFile, std::filesystem::path audioFile, std::filesystem::path outputMp4File) {
        m_ptr = ptr;
        m_videoFile = videoFile;
        m_audioFile = audioFile;
        m_outputMp4File = outputMp4File;
    }

    void setResult(const geode::Result<void>& result) {m_result = geode::Result(result);}
    geode::Result<void> getResult() {return m_result;}

    void* getPtr() {return m_ptr;}

    std::filesystem::path getVideoFile() {return m_videoFile;}
    std::filesystem::path getAudioFile() {return m_audioFile;}
    std::filesystem::path getOutputMp4File() {return m_outputMp4File;}

private:
    std::filesystem::path m_videoFile;
    std::filesystem::path m_audioFile;
    std::filesystem::path m_outputMp4File;
    void* m_ptr;
    geode::Result<void> m_result = geode::Ok();
};

class MixVideoRawEvent : public geode::Event {
public:
    MixVideoRawEvent(void* ptr, std::filesystem::path videoFile, const std::vector<float>& raw, std::filesystem::path outputMp4File) {
        m_ptr = ptr;
        m_videoFile = videoFile;
        m_raw = &raw;
        m_outputMp4File = outputMp4File;
    }

    void setResult(const geode::Result<void>& result) {m_result = geode::Result(result);}
    geode::Result<void> getResult() {return m_result;}

    void* getPtr() {return m_ptr;}

    std::filesystem::path getVideoFile() {return m_videoFile;}
    const std::vector<float>& getRaw() {return *m_raw;}
    std::filesystem::path getOutputMp4File() {return m_outputMp4File;}

private:
    std::filesystem::path m_videoFile;
    const std::vector<float>* m_raw;
    std::filesystem::path m_outputMp4File;
    void* m_ptr;
    geode::Result<void> m_result = geode::Ok();
};


class Recorder {
public:
    Recorder() {
        CreateRecorderEvent createEvent;
        createEvent.post();
        m_ptr = createEvent.getPtr();
    }

    ~Recorder() {
        DeleteRecorderEvent deleteEvent(m_ptr);
        deleteEvent.post();
    }

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
    geode::Result<void> init(const RenderSettings& settings) {
        InitRecorderEvent initEvent(m_ptr, settings);
        initEvent.post();
        return initEvent.getResult();
    }
    /**
     * @brief Stops the recording process and finalizes the output file.
     *
     * This function ensures that all buffered frames are written to the output file,
     * releases allocated resources, and properly closes the output file.
     */
    void stop() {
        StopRecorderEvent stopEvent(m_ptr);
        stopEvent.post();
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
    geode::Result<void> writeFrame(const std::vector<uint8_t>& frameData) {
        WriteFrameRecorderEvent writeFrameEvent(m_ptr, frameData);
        writeFrameEvent.post();
        return writeFrameEvent.getResult();
    }

    /**
     * @brief Retrieves a list of available codecs for video encoding.
     *
     * This function iterates through all available codecs in FFmpeg and 
     * returns a sorted vector of codec names.
     * 
     * @return A vector representing the names of available codecs.
     */
    std::vector<std::string> getAvailableCodecs() {
        CodecRecorderEvent codecEvent(m_ptr);
        codecEvent.post();
        return codecEvent.getCodecs();
    }
private:
    void* m_ptr = nullptr;
};

class AudioMixer {
public:
    AudioMixer() {
        CreateMixerEvent createEvent;
        createEvent.post();
        m_ptr = createEvent.getPtr();
    }

    ~AudioMixer() {
        DeleteMixerEvent deleteEvent(m_ptr);
        deleteEvent.post();
    }

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
    geode::Result<void> mixVideoAudio(std::filesystem::path videoFile, std::filesystem::path audioFile, std::filesystem::path outputMp4File) {
        MixVideoAudioEvent mixEvent(m_ptr, videoFile, audioFile, outputMp4File);
        mixEvent.post();
        return mixEvent.getResult();
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
    geode::Result<void> mixVideoRaw(const std::filesystem::path& videoFile, const std::vector<float>& raw, const std::filesystem::path &outputMp4File) {
        MixVideoRawEvent mixEvent(m_ptr, videoFile, raw, outputMp4File);
        mixEvent.post();
        return mixEvent.getResult();
    }
private:
    void* m_ptr = nullptr;
};

}