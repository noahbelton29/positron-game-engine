//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/renderer/opengl/buffer_layout.h"
#include "positron/renderer/opengl/index_buffer.h"
#include "positron/renderer/opengl/vertex_buffer.h"

#include <memory>

namespace Positron {
    class POSITRON_API VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray &)            = delete;
        VertexArray &operator=(const VertexArray &) = delete;

        void bind() const;
        void unbind() const;

        void addVertexBuffer(std::shared_ptr<VertexBuffer> vbo, const BufferLayout &layout);
        void setIndexBuffer(std::shared_ptr<IndexBuffer> ibo);

        [[nodiscard]] const std::shared_ptr<IndexBuffer> &getIndexBuffer() const { return ibo_; }
        [[nodiscard]] unsigned int                        getID() const { return id_; }

    private:
        unsigned int                  id_          = 0;
        unsigned int                  attribIndex_ = 0;
        std::shared_ptr<VertexBuffer> vbo_;
        std::shared_ptr<IndexBuffer>  ibo_;
    };
} // namespace Positron
