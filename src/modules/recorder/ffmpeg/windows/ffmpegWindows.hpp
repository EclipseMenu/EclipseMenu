#pragma once
#ifndef GEODE_IS_ANDROID
#include "../ffmpegcli.hpp"
#include <subprocess.hpp>

namespace eclipse::recorder {

    class ffmpegWindows : public ffmpegCLI {
    public:
        void open(const std::string& cmd) override;
        bool close() override;
        void write(const void* data, size_t size) override;
    private:
        subprocess::Popen m_process;
    };

}
#endif
