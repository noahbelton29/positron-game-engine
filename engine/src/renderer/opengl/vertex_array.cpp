//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/opengl/vertex_array.h"

#include <glad/glad.h>

namespace Positron {
    VertexArray::VertexArray() { glGenVertexArrays(1, &id_); }

    VertexArray::~VertexArray() { glDeleteVertexArrays(1, &id_); }

    void VertexArray::bind() const { glBindVertexArray(id_); }

    void VertexArray::unbind() const { glBindVertexArray(0); }

    void VertexArray::addVertexBuffer(std::shared_ptr<VertexBuffer> vbo, const BufferLayout &layout) {
        glBindVertexArray(id_);
        vbo->bind();

        for (const auto &element: layout.getElements()) {
            glEnableVertexAttribArray(attribIndex_);
            glVertexAttribPointer(attribIndex_, element.getComponentCount(), element.getGLBaseType(),
                                  element.normalized ? GL_TRUE : GL_FALSE, static_cast<GLsizei>(layout.getStride()),
                                  reinterpret_cast<const void *>(element.offset));
            ++attribIndex_;
        }

        vbo_ = std::move(vbo);
        glBindVertexArray(0);
    }

    void VertexArray::setIndexBuffer(std::shared_ptr<IndexBuffer> ibo) {
        glBindVertexArray(id_);
        ibo->bind();
        ibo_ = std::move(ibo);
        glBindVertexArray(0);
    }
} // namespace Positron
