//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include <cstdint>
#include <vector>

namespace Positron {
    struct Vertex {
        float position[3];
        float color[3];
        float normal[3];
        float uv[2];
    };

    struct GeometryData {
        std::vector<Vertex>   vertices;
        std::vector<uint32_t> indices;
    };

    namespace Geometry {
        GeometryData cube(float size = 1.0f);
        GeometryData plane(float width = 1.0f, float height = 1.0f, int subdivisionsX = 1, int subdivisionsZ = 1);
    } // namespace Geometry
} // namespace Positron
