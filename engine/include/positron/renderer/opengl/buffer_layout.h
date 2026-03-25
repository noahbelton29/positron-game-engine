//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace Positron {
    enum class ShaderDataType {
        Float,
        Float2,
        Float3,
        Float4,
        Int,
        Int2,
        Int3,
        Int4,
        Bool,
    };

    struct POSITRON_API BufferElement {
        ShaderDataType type;
        bool           normalized = false;
        std::size_t    offset     = 0;

        [[nodiscard]] int          getComponentCount() const;
        [[nodiscard]] unsigned int getGLBaseType() const;
        [[nodiscard]] std::size_t  getSize() const;
    };

    class POSITRON_API BufferLayout {
    public:
        BufferLayout() = default;
        BufferLayout(std::initializer_list<BufferElement> elements);

        [[nodiscard]] const std::vector<BufferElement> &getElements() const { return elements_; }
        [[nodiscard]] std::size_t                       getStride() const { return stride_; }

    private:
        void calculateOffsetsAndStride();

        std::vector<BufferElement> elements_;
        std::size_t                stride_ = 0;
    };
} // namespace Positron
