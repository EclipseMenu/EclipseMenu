#pragma once

#include <cocos2d.h>

namespace eclipse::recorder {
    class RenderTexture {
    public:
        uint32_t m_width, m_height;
        int m_old_fbo, m_old_rbo;
        uint32_t m_fbo;
        cocos2d::CCTexture2D* m_texture;

    public:
        void begin();
        void end() const;
        void capture(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& hasDataFlag);
    };
}
