#pragma once

#include <string>

namespace eclipse::recorder {

    class ffmpegCLI {
        public:
            virtual void open(const std::string& cmd) = 0;
            virtual bool close() = 0;
            virtual void write(const void* data, size_t size) = 0;
    };

};