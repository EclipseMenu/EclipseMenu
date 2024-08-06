#pragma once

#include <modules/gui/gui.hpp>

namespace eclipse::gui::blur {
    struct Shader {
        GLuint vertex = 0;
        GLuint fragment = 0;
        GLuint program = 0;

        geode::Result<std::string> compile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
        geode::Result<std::string> link();
        void cleanup();
    };

    struct RenderTexture {
        GLuint fbo = 0;
        GLuint tex = 0;
        GLuint rbo = 0;

        void setup(GLsizei width, GLsizei height);
        void resize(GLsizei width, GLsizei height) const;
        void cleanup();
    };

    void init();
    void update(float dt);
}

