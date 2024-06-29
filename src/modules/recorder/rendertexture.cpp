#include "rendertexture.hpp"

#include <Geode/cocos/platform/CCGL.h>

namespace eclipse::recorder {

    void RenderTexture::begin() {
#ifdef GEODE_IS_WINDOWS
        glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &m_old_fbo);

        m_texture = new cocos2d::CCTexture2D;

        {
            auto data = malloc(m_width * m_height * 3);
            memset(data, 0, m_width * m_height * 3);
            m_texture->initWithData(data, cocos2d::kCCTexture2DPixelFormat_RGB888, m_width, m_height,
                                    cocos2d::CCSize(static_cast<float>(m_width), static_cast<float>(m_height)));
            free(data);
        }

        glGetIntegerv(GL_RENDERBUFFER_BINDING_EXT, &m_old_rbo);

        glGenFramebuffersEXT(1, &m_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

        glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture->getName(), 0);

        m_texture->setAliasTexParameters();

        glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, m_old_rbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_old_fbo);
#endif
    }

    void RenderTexture::end() {
        m_texture->release();
    }

    void RenderTexture::capture(std::mutex &lock, std::vector<uint8_t> &data, volatile bool &hasDataFlag) {
#ifdef GEODE_IS_WINDOWS
        auto director = cocos2d::CCDirector::sharedDirector();

        glViewport(0, 0, m_width, m_height);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING_EXT, &m_old_fbo);
        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_fbo);

        PlayLayer::get()->visit();

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        lock.lock();
        hasDataFlag = true;
        glReadPixels(0, 0, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, data.data());
        lock.unlock();

        glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, m_old_fbo);
        director->setViewport();
#endif
    }

};
