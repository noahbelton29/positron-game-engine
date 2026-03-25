//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/opengl/vertex_buffer.h"

#include <glad/glad.h>

namespace Positron {
    VertexBuffer::VertexBuffer(const float *data, const std::size_t size) {
        glGenBuffers(1, &id_);
        glBindBuffer(GL_ARRAY_BUFFER, id_);
        glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(size), data, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    VertexBuffer::~VertexBuffer() { glDeleteBuffers(1, &id_); }

    void VertexBuffer::bind() const { glBindBuffer(GL_ARRAY_BUFFER, id_); }

    void VertexBuffer::unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

    void VertexBuffer::update(const float *data, const std::size_t size) const {
        glBindBuffer(GL_ARRAY_BUFFER, id_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, static_cast<GLsizeiptr>(size), data);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
} // namespace Positron
