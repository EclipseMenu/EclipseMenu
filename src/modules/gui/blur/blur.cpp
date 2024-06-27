#include "blur.hpp"

using namespace geode::prelude;

// COMPLETELY stolen from cgytrus/SimplePatchLoader (with permission)

#ifdef GEODE_IS_DESKTOP

Result<std::string> Shader::compile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath) {
    auto vertexSource = file::readString(vertexPath);
        if (!vertexSource)
            return Err("failed to read vertex shader at path {}: {}", vertexPath.string(), vertexSource.unwrapErr());

        auto fragmentSource = file::readString(fragmentPath);
        if (!fragmentSource)
            return Err("failed to read fragment shader at path {}: {}", fragmentPath.string(), fragmentSource.unwrapErr());

        auto getShaderLog = [](GLuint id) -> std::string {
            GLint length, written;
            glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
            if (length <= 0)
                return "";
            auto stuff = new char[length + 1];
            glGetShaderInfoLog(id, length, &written, stuff);
            std::string result(stuff);
            delete[] stuff;
            return result;
        };
        GLint res;

        vertex = glCreateShader(GL_VERTEX_SHADER);
        auto oglSucks = vertexSource.unwrap().c_str();
        glShaderSource(vertex, 1, &oglSucks, nullptr);
        glCompileShader(vertex);
        auto vertexLog = getShaderLog(vertex);

        glGetShaderiv(vertex, GL_COMPILE_STATUS, &res);
        if(!res) {
            glDeleteShader(vertex);
            vertex = 0;
            return Err("vertex shader compilation failed:\n{}", vertexLog);
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        oglSucks = fragmentSource.unwrap().c_str();
        glShaderSource(fragment, 1, &oglSucks, nullptr);
        glCompileShader(fragment);
        auto fragmentLog = getShaderLog(fragment);

        glGetShaderiv(fragment, GL_COMPILE_STATUS, &res);
        if(!res) {
            glDeleteShader(vertex);
            glDeleteShader(fragment);
            vertex = 0;
            fragment = 0;
            return Err("fragment shader compilation failed:\n{}", fragmentLog);
        }

        program = glCreateProgram();
        glAttachShader(program, vertex);
        glAttachShader(program, fragment);

        return Ok(fmt::format(
            "shader compilation successful. logs:\nvert:\n{}\nfrag:\n{}",
            vertexLog, fragmentLog
        ));
}

Result<std::string> Shader::link() {
    if (!vertex)
        return Err("vertex shader not compiled");
    if (!fragment)
        return Err("fragment shader not compiled");

    auto getProgramLog = [](GLuint id) -> std::string {
        GLint length, written;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &length);
        if (length <= 0)
            return "";
        auto stuff = new char[length + 1];
        glGetProgramInfoLog(id, length, &written, stuff);
        std::string result(stuff);
        delete[] stuff;
        return result;
    };
    GLint res;

    glLinkProgram(program);
    auto programLog = getProgramLog(program);

    glDeleteShader(vertex);
    glDeleteShader(fragment);
    vertex = 0;
    fragment = 0;

    glGetProgramiv(program, GL_LINK_STATUS, &res);
    if(!res) {
        glDeleteProgram(program);
        program = 0;
        return Err("shader link failed:\n{}", programLog);
    }

    return Ok(fmt::format("shader link successful. log:\n{}", programLog));
}

void Shader::cleanup() {
    if (program)
        glDeleteProgram(program);
    program = 0;
}

void RenderTexture::setup(GLsizei width, GLsizei height) {
    GLint drawFbo = 0;
    GLint readFbo = 0;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFbo);
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFbo);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        log::error("pp fbo not complete, uh oh! i guess i will have to cut off ur pp now");
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);
}

void RenderTexture::cleanup() {
    if (fbo)
        glDeleteFramebuffers(1, &fbo);
    if (tex)
        glDeleteTextures(1, &tex);
    if (rbo)
        glDeleteRenderbuffers(1, &rbo);
    fbo = 0;
    tex = 0;
    rbo = 0;
}

void setupPostProcess() {
    auto size = CCDirector::get()->getOpenGLView()->getFrameSize() * geode::utils::getDisplayFactor();

    ppRt0.setup((GLsizei)size.width, (GLsizei)size.height);
    ppRt1.setup((GLsizei)size.width, (GLsizei)size.height);

    GLfloat ppVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
        1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
        1.0f, -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f,  1.0f, 1.0f
    };
    glGenVertexArrays(1, &ppVao);
    glGenBuffers(1, &ppVbo);
    glBindVertexArray(ppVao);
    glBindBuffer(GL_ARRAY_BUFFER, ppVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ppVertices), &ppVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    auto vertexPath = (std::string)CCFileUtils::get()->fullPathForFilename("pp-vert.glsl"_spr, false);
    auto fragmentPath = (std::string)CCFileUtils::get()->fullPathForFilename("pp-frag.glsl"_spr, false);

    auto res = ppShader.compile(vertexPath, fragmentPath);
    if (!res) {
        log::error("Failed to compile shader: {}", res.unwrapErr());
        return;
    }
    log::info("{}", res.unwrap());

    glBindAttribLocation(ppShader.program, 0, "aPosition");
    glBindAttribLocation(ppShader.program, 1, "aTexCoords");

    res = ppShader.link();
    if (!res) {
        log::error("Failed to link shader: {}", res.unwrapErr());
        return;
    }
    log::info("{}", res.unwrap());

    ccGLUseProgram(ppShader.program);
    glUniform1i(glGetUniformLocation(ppShader.program, "screen"), 0);
    glUniform2f(glGetUniformLocation(ppShader.program, "screenSize"), size.width, size.height);
    ppShaderFast = glGetUniformLocation(ppShader.program, "fast");
    ppShaderFirst = glGetUniformLocation(ppShader.program, "first");
    ppShaderRadius = glGetUniformLocation(ppShader.program, "radius");
}

void cleanupPostProcess() {
    ppRt0.cleanup();
    ppRt1.cleanup();

    if (ppVao)
        glDeleteVertexArrays(1, &ppVao);
    if (ppVbo)
        glDeleteBuffers(1, &ppVbo);
    ppVao = 0;
    ppVbo = 0;

    ppShader.cleanup();
    ppShaderFast = 0;
    ppShaderFirst = 0;
    ppShaderRadius = 0;
}

// hook time yippee

#include <Geode/modify/CCEGLViewProtocol.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/CCNode.hpp>
#include <imgui-cocos.hpp>

class $modify(CCNode) {
    void visit() {
        if ((CCNode*)this != CCDirector::get()->getRunningScene() || ppShader.program == 0) {
            CCNode::visit();
            return;
        }
        float blur = 0.05 * (1.f - std::cos((float)std::numbers::pi * blurTimer)) * 0.5f;
        if (blur == 0.f) {
            CCNode::visit();
            return;
        }

        GLint drawFbo = 0;
        GLint readFbo = 0;
        glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &drawFbo);
        glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &readFbo);

        glBindFramebuffer(GL_FRAMEBUFFER, ppRt0.fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        CCNode::visit();

        glBindVertexArray(ppVao);
        ccGLUseProgram(ppShader.program);
        glUniform1i(ppShaderFast, true);
        glUniform1f(ppShaderRadius, blur);

        glBindFramebuffer(GL_FRAMEBUFFER, ppRt1.fbo);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

        glUniform1i(ppShaderFirst, GL_TRUE);
        glBindTexture(GL_TEXTURE_2D, ppRt0.tex);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, drawFbo);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFbo);

        glUniform1i(ppShaderFirst, GL_FALSE);
        glBindTexture(GL_TEXTURE_2D, ppRt1.tex);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);
    }
};

class $modify(CCEGLViewProtocol) {
    void setFrameSize(float width, float height) {
        CCEGLViewProtocol::setFrameSize(width, height);
        if (!CCDirector::get()->getOpenGLView())
            return;
        cleanupPostProcess();
        setupPostProcess();
    }
};

class $modify(GameManager) {
    void reloadAllStep5() {
        GameManager::reloadAllStep5();
        cleanupPostProcess();
        setupPostProcess();
    }
};

$on_mod(Loaded) {
    Loader::get()->queueInMainThread([]() {
        cleanupPostProcess();
        setupPostProcess();
    });
}
$on_mod(Unloaded) {
    cleanupPostProcess();
}

class $modify(CCScheduler) {
    void update(float dt) {
        CCScheduler::update(dt);
        if (eclipse::gui::Engine::get()->isToggled())
            blurTimer += CCDirector::get()->getDeltaTime() / 0.1f;
        else
            blurTimer -= CCDirector::get()->getDeltaTime() / 0.1f;
        if (blurTimer < 0.f) blurTimer = 0.f;
        if (blurTimer > 1.f) blurTimer = 1.f;

        if (blurTimer == 0.f)
            return;
    }
};

#endif