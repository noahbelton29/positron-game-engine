//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <string>
#include <unordered_map>

namespace Positron {
    class POSITRON_API GLShader {
    public:
        GLShader(const std::string &vertexPath, const std::string &fragmentPath);
        ~GLShader();

        GLShader(const GLShader &)            = delete;
        GLShader &operator=(const GLShader &) = delete;

        [[nodiscard]] static GLShader *fromSource(const std::string &vertSrc, const std::string &fragSrc);

        void bind() const;
        void unbind() const;

        void setInt(const std::string &name, int value);
        void setFloat(const std::string &name, float value);
        void setVec2(const std::string &name, float x, float y);
        void setVec3(const std::string &name, float x, float y, float z);
        void setVec4(const std::string &name, float x, float y, float z, float w);
        void setMat4(const std::string &name, const float *matrix);

        [[nodiscard]] unsigned int getID() const { return id_; }

    private:
        GLShader() = default;

        unsigned int id_ = 0;

        void compile(const std::string &vertSrc, const std::string &fragSrc);

        [[nodiscard]] static std::string  loadFile(const std::string &path);
        [[nodiscard]] static unsigned int compileStage(unsigned int type, const std::string &source);

        int                                  getUniformLocation(const std::string &name);
        std::unordered_map<std::string, int> uniformCache_;
    };
} // namespace Positron
