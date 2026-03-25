//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/mesh_factory.h"
#include "positron/renderer/opengl/gl_mesh.h"

namespace Positron {
    std::unique_ptr<Mesh> MeshFactory::create(const RenderAPI api, const GeometryData &data) {
        switch (api) {
            case RenderAPI::OpenGL:
                return std::make_unique<GLMesh>(data);
            default:
                return nullptr;
        }
    }

    std::unique_ptr<Mesh> MeshFactory::cube(const RenderAPI api, const float size) {
        return create(api, Geometry::cube(size));
    }

    std::unique_ptr<Mesh> MeshFactory::plane(const RenderAPI api, const float w, const float h, const int subX,
                                             const int subZ) {
        return create(api, Geometry::plane(w, h, subX, subZ));
    }
} // namespace Positron
