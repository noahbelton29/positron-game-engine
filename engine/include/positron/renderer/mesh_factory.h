//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/renderer/geometry.h"
#include "positron/renderer/mesh.h"
#include "positron/renderer/renderer.h"

#include <memory>

namespace Positron {
    class POSITRON_API MeshFactory {
    public:
        static std::unique_ptr<Mesh> create(RenderAPI api, const GeometryData &data);

        static std::unique_ptr<Mesh> cube(RenderAPI api, float size = 1.0f);
        static std::unique_ptr<Mesh> plane(RenderAPI api, float w = 1.0f, float h = 1.0f, int subX = 1, int subZ = 1);
    };
} // namespace Positron
