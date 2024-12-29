#include "rendertexture.hpp"

#include <Geode/cocos/platform/win32/CCGL.h>
#include <Geode/binding/PlayLayer.hpp>
#include <modules/utils/SingletonCache.hpp>

namespace eclipse::recorder {

    void RenderTexture::begin() {
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_old_fbo);

        m_texture = new cocos2d::CCTexture2D;

        {
            std::unique_ptr<char, void(*)(void*)> data(static_cast<char*>(malloc(m_width * m_height * 4)), free);

            memset(data.get(), 0, m_width * m_height * 4);
            m_texture->initWithData(
                data.get(),
                cocos2d::kCCTexture2DPixelFormat_RGBA8888,
                m_width, m_height,
                cocos2d::CCSize(static_cast<float>(m_width), static_cast<float>(m_height))
            );
        }

        glGetIntegerv(GL_RENDERBUFFER_BINDING, &m_old_rbo);

        glGenFramebuffers(1, &m_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_texture->getName(), 0);

        m_texture->setAliasTexParameters();

        glBindRenderbuffer(GL_RENDERBUFFER, m_old_rbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_old_fbo);
    }

    void RenderTexture::end() const {
        m_texture->release();
    }

    void RenderTexture::capture(std::mutex& lock, std::vector<uint8_t>& data, volatile bool& hasDataFlag) {
        auto director = utils::get<cocos2d::CCDirector>();

        glViewport(0, 0, m_width, m_height);

        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &m_old_fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

        utils::get<PlayLayer>()->visit();

        glPixelStorei(GL_PACK_ALIGNMENT, 1);
        lock.lock();
        hasDataFlag = true;
        glReadPixels(0, 0, m_width, m_height, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
        lock.unlock();

        glBindFramebuffer(GL_FRAMEBUFFER, m_old_fbo);
        director->setViewport();
    }

};
