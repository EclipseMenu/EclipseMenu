#include "ffmpegWindows.hpp"
#ifdef GEODE_IS_WINDOWS
namespace eclipse::recorder {

    void ffmpegWindows::open(const std::string& cmd) {
        m_process = subprocess::Popen(cmd);
    }

    void ffmpegWindows::write(const void* data, size_t size) {
        m_process.m_stdin.write(data, size);
    }

    bool ffmpegWindows::close() {
        try {
            m_process.close();
            return true;
        }
        catch (...) {
            return false;
        }
    }

}
#endif
