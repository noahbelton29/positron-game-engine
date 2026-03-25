//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/opengl/gl_mesh.h"

namespace Positron {
    GLMesh::GLMesh(const GeometryData &data) {
        const auto vbo = std::make_shared<VertexBuffer>(reinterpret_cast<const float *>(data.vertices.data()),
                                                        data.vertices.size() * sizeof(Vertex));

        const auto ibo = std::make_shared<IndexBuffer>(data.indices.data(), data.indices.size());

        const BufferLayout layout = {
                {ShaderDataType::Float3}, // position
                {ShaderDataType::Float3}, // color
                {ShaderDataType::Float3}, // normal
                {ShaderDataType::Float2}, // uv
        };

        vao_ = std::make_unique<VertexArray>();
        vao_->addVertexBuffer(vbo, layout);
        vao_->setIndexBuffer(ibo);
    }

    void GLMesh::bind() const { vao_->bind(); }
    void GLMesh::unbind() const { vao_->unbind(); }
    int  GLMesh::getIndexCount() const { return vao_->getIndexBuffer()->getCount(); }
} // namespace Positron
