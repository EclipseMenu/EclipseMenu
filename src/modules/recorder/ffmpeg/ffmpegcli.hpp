#pragma once

#ifndef GEODE_IS_ANDROID
#include <string>

namespace eclipse::recorder {

    class ffmpegCLI {
    public:
        virtual ~ffmpegCLI() = default; 
        virtual void open(const std::string& cmd) = 0;
        virtual bool close() = 0;
        virtual void write(const void* data, size_t size) = 0;
    };

};
#endif
