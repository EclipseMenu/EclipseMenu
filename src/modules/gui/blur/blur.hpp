#pragma once

#include <numbers>
#include <Geode/Geode.hpp>
#include <modules/gui/gui.hpp>

using namespace geode::prelude;

struct Shader {
    GLuint vertex = 0;
    GLuint fragment = 0;
    GLuint program = 0;

    Result<std::string> compile(const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
    Result<std::string> link();
    void cleanup();
};

struct RenderTexture {
    GLuint fbo = 0;
    GLuint tex = 0;
    GLuint rbo = 0;

    void setup(GLsizei width, GLsizei height);
    void cleanup();
};

RenderTexture ppRt0;
RenderTexture ppRt1;
GLuint ppVao = 0;
GLuint ppVbo = 0;
Shader ppShader;
GLint ppShaderFast = 0;
GLint ppShaderFirst = 0;
GLint ppShaderRadius = 0;

float blurTimer = 0.f;