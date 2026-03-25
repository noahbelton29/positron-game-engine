//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <cstddef>

namespace Positron {
    class POSITRON_API VertexBuffer {
    public:
        VertexBuffer(const float *data, std::size_t size);
        ~VertexBuffer();

        VertexBuffer(const VertexBuffer &)            = delete;
        VertexBuffer &operator=(const VertexBuffer &) = delete;

        void bind() const;
        void unbind() const;

        void update(const float *data, std::size_t size) const;

        [[nodiscard]] unsigned int getID() const { return id_; }

    private:
        unsigned int id_ = 0;
    };
} // namespace Positron
