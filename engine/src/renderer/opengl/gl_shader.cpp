//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/opengl/gl_shader.h"
#include "positron/core/log.h"

#include <glad/glad.h>

#include <fstream>
#include <sstream>

namespace Positron {
    GLShader::GLShader(const std::string &vertexPath, const std::string &fragmentPath) {
        const std::string vertSrc = loadFile(vertexPath);
        const std::string fragSrc = loadFile(fragmentPath);

        if (vertSrc.empty() || fragSrc.empty()) {
            POSITRON_ERROR("Failed to load shader files: {} {}", vertexPath, fragmentPath);
            return;
        }

        compile(vertSrc, fragSrc);
        POSITRON_INFO("Shader loaded: {} + {}", vertexPath, fragmentPath);
    }

    GLShader *GLShader::fromSource(const std::string &vertSrc, const std::string &fragSrc) {
        auto *shader = new GLShader();
        shader->compile(vertSrc, fragSrc);
        return shader;
    }

    void GLShader::compile(const std::string &vertSrc, const std::string &fragSrc) {
        const unsigned int vert = compileStage(GL_VERTEX_SHADER, vertSrc);
        const unsigned int frag = compileStage(GL_FRAGMENT_SHADER, fragSrc);

        id_ = glCreateProgram();
        glAttachShader(id_, vert);
        glAttachShader(id_, frag);
        glLinkProgram(id_);

        int success;
        glGetProgramiv(id_, GL_LINK_STATUS, &success);
        if (!success) {
            char log[512];
            glGetProgramInfoLog(id_, 512, nullptr, log);
            POSITRON_ERROR("Shader link error: {}", log);
        }

        glDeleteShader(vert);
        glDeleteShader(frag);
    }

    GLShader::~GLShader() {
        if (id_)
            glDeleteProgram(id_);
    }

    void GLShader::bind() const { glUseProgram(id_); }
    void GLShader::unbind() const { glUseProgram(0); }

    void GLShader::setInt(const std::string &name, const int value) { glUniform1i(getUniformLocation(name), value); }

    void GLShader::setFloat(const std::string &name, const float value) {
        glUniform1f(getUniformLocation(name), value);
    }

    void GLShader::setVec2(const std::string &name, const float x, const float y) {
        glUniform2f(getUniformLocation(name), x, y);
    }

    void GLShader::setVec3(const std::string &name, const float x, const float y, const float z) {
        glUniform3f(getUniformLocation(name), x, y, z);
    }

    void GLShader::setVec4(const std::string &name, const float x, const float y, const float z, const float w) {
        glUniform4f(getUniformLocation(name), x, y, z, w);
    }

    void GLShader::setMat4(const std::string &name, const float *matrix) {
        glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix);
    }

    int GLShader::getUniformLocation(const std::string &name) {
        if (uniformCache_.contains(name))
            return uniformCache_[name];
        const int location = glGetUniformLocation(id_, name.c_str());
        if (location == -1)
            POSITRON_WARN("Uniform '{}' not found in shader", name);
        uniformCache_[name] = location;
        return location;
    }

    std::string GLShader::loadFile(const std::string &path) {
        std::ifstream file(path);
        if (!file.is_open()) {
            POSITRON_ERROR("Could not open shader file: {}", path);
            return "";
        }
        std::ostringstream ss;
        ss << file.rdbuf();
        return ss.str();
    }

    unsigned int GLShader::compileStage(const unsigned int type, const std::string &source) {
        const unsigned int id  = glCreateShader(type);
        const char        *src = source.c_str();
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int success;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success) {
            char log[512];
            glGetShaderInfoLog(id, 512, nullptr, log);
            POSITRON_ERROR("Shader compile error ({}): {}", type == GL_VERTEX_SHADER ? "vertex" : "fragment", log);
        }
        return id;
    }
} // namespace Positron
