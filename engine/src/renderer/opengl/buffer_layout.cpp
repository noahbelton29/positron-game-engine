//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/opengl/buffer_layout.h"

#include <glad/glad.h>

namespace Positron {
    static std::size_t shaderDataTypeSize(const ShaderDataType type) {
        switch (type) {
            case ShaderDataType::Float:
                return 4;
            case ShaderDataType::Float2:
                return 4 * 2;
            case ShaderDataType::Float3:
                return 4 * 3;
            case ShaderDataType::Float4:
                return 4 * 4;
            case ShaderDataType::Int:
                return 4;
            case ShaderDataType::Int2:
                return 4 * 2;
            case ShaderDataType::Int3:
                return 4 * 3;
            case ShaderDataType::Int4:
                return 4 * 4;
            case ShaderDataType::Bool:
                return 1;
        }
        return 0;
    }

    int BufferElement::getComponentCount() const {
        switch (type) {
            case ShaderDataType::Float:
                return 1;
            case ShaderDataType::Float2:
                return 2;
            case ShaderDataType::Float3:
                return 3;
            case ShaderDataType::Float4:
                return 4;
            case ShaderDataType::Int:
                return 1;
            case ShaderDataType::Int2:
                return 2;
            case ShaderDataType::Int3:
                return 3;
            case ShaderDataType::Int4:
                return 4;
            case ShaderDataType::Bool:
                return 1;
        }
        return 0;
    }

    unsigned int BufferElement::getGLBaseType() const {
        switch (type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
                return GL_FLOAT;
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
                return GL_INT;
            case ShaderDataType::Bool:
                return GL_BOOL;
        }
        return 0;
    }

    std::size_t BufferElement::getSize() const { return shaderDataTypeSize(type); }

    BufferLayout::BufferLayout(const std::initializer_list<BufferElement> elements) : elements_(elements) {
        calculateOffsetsAndStride();
    }

    void BufferLayout::calculateOffsetsAndStride() {
        std::size_t offset = 0;
        stride_            = 0;
        for (auto &e: elements_) {
            e.offset = offset;
            offset += e.getSize();
            stride_ += e.getSize();
        }
    }
} // namespace Positron
