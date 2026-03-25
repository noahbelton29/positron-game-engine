//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <cstddef>
#include <cstdint>

namespace Positron {
    class POSITRON_API IndexBuffer {
    public:
        IndexBuffer(const uint32_t *indices, std::size_t count);
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer &)            = delete;
        IndexBuffer &operator=(const IndexBuffer &) = delete;

        void bind() const;
        void unbind() const;

        [[nodiscard]] unsigned int getID() const { return id_; }
        [[nodiscard]] int          getCount() const { return count_; }

    private:
        unsigned int id_    = 0;
        int          count_ = 0;
    };
} // namespace Positron
