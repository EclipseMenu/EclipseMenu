#include "rendertexture.hpp"

#include <Geode/binding/PlayLayer.hpp>
#include <Geode/cocos/platform/win32/CCGL.h>
#include <modules/utils/SingletonCache.hpp>

namespace eclipse::recorder {
    void RenderTexture::begin() {
        // Save the old FBO
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFBO);

        // Create a new texture
        constexpr auto bitsPerPixel = 32;
        auto bytesPerRow = m_width * bitsPerPixel / 8;
        if (bytesPerRow % 8 == 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
        } else if (bytesPerRow % 4 == 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        } else if (bytesPerRow % 2 == 0) {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        } else {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        }

        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

        // Save the old RBO
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_oldRBO);

        // Create a new FBO
        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        // Attach the texture to the FBO
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture, 0);

        // Set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        // Unbind the FBO and RBO
        glBindRenderbuffer(GL_RENDERBUFFER, m_oldRBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_oldFBO);
    }

    void RenderTexture::end() const {
        if (m_texture) glDeleteTextures(1, &m_texture);
        if (m_fbo) glDeleteFramebuffers(1, &m_fbo);
    }

    void RenderTexture::capture(cocos2d::CCNode* node, std::span<uint8_t> buffer, std::mutex& lock, std::condition_variable& cv, volatile bool& hasDataFlag) {
        glViewport(0, 0, m_width, m_height);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_oldFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        auto director = utils::get<cocos2d::CCDirector>();
        director->setProjection(cocos2d::kCCDirectorProjectionCustom);
        node->visit();
        director->setProjection(cocos2d::kCCDirectorProjection2D);

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        {
            std::unique_lock l(lock);
            glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, buffer.data());
            hasDataFlag = true;
        }
        cv.notify_one(); // calling notify_one() outside of the lock, to avoid locking the mutex twice

        glBindFramebuffer(GL_FRAMEBUFFER, m_oldFBO);
        director->setViewport();
    }
}
