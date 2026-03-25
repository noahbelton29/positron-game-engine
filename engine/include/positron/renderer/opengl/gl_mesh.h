//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/renderer/geometry.h"
#include "positron/renderer/mesh.h"
#include "positron/renderer/opengl/buffer_layout.h"
#include "positron/renderer/opengl/vertex_array.h"

#include <memory>

namespace Positron {
    class POSITRON_API GLMesh : public Mesh {
    public:
        explicit GLMesh(const GeometryData &data);

        void bind() const override;
        void unbind() const override;
        int  getIndexCount() const override;

    private:
        std::unique_ptr<VertexArray> vao_;
    };
} // namespace Positron
