#pragma once

#include <Geode/Geode.hpp>
#include <modules/gui/gui.hpp>

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
    void cleanup();
};

