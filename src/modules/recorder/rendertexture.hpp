#pragma once

#include <cocos2d.h>
#include "spinlock.hpp"

namespace eclipse::recorder {
    class RenderTexture {
    protected:
        uint32_t m_width = 0, m_height = 0;
        GLint m_oldFBO = 0, m_oldRBO = 0;
        GLuint m_fbo = 0;
        GLuint m_texture = 0;

    public:
        RenderTexture() = default;
        RenderTexture(uint32_t width, uint32_t height) : m_width(width), m_height(height) {}
        ~RenderTexture() { this->end(); }

        void begin();
        void end() const;

        void capture(cocos2d::CCNode* node, std::span<uint8_t> buffer, utils::spinlock& frameReady);
    };
}
