//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/geometry.h"

namespace Positron::Geometry {
    GeometryData cube(const float size) {
        const float h = size * 0.5f;

        const float faces[6][4][3] = {
                // front
                {{-h, -h, h}, {h, -h, h}, {h, h, h}, {-h, h, h}},
                // back
                {{h, -h, -h}, {-h, -h, -h}, {-h, h, -h}, {h, h, -h}},
                // left
                {{-h, -h, -h}, {-h, -h, h}, {-h, h, h}, {-h, h, -h}},
                // right
                {{h, -h, h}, {h, -h, -h}, {h, h, -h}, {h, h, h}},
                // top
                {{-h, h, h}, {h, h, h}, {h, h, -h}, {-h, h, -h}},
                // bottom
                {{-h, -h, -h}, {h, -h, -h}, {h, -h, h}, {-h, -h, h}},
        };

        GeometryData data;
        for (int f = 0; f < 6; ++f) {
            const uint32_t base = static_cast<uint32_t>(data.vertices.size());
            for (int v = 0; v < 4; ++v) {
                const float     normals[6][3] = {{0, 0, 1}, {0, 0, -1}, {-1, 0, 0}, {1, 0, 0}, {0, 1, 0}, {0, -1, 0}};
                constexpr float uvs[4][2]     = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
                const float     colors[6][3]  = {
                        {0.8f, 0.2f, 0.2f}, {0.2f, 0.8f, 0.2f}, {0.2f, 0.2f, 0.8f},
                        {0.8f, 0.8f, 0.2f}, {0.8f, 0.2f, 0.8f}, {0.2f, 0.8f, 0.8f},
                };

                Vertex vert{};
                vert.position[0] = faces[f][v][0];
                vert.position[1] = faces[f][v][1];
                vert.position[2] = faces[f][v][2];
                vert.color[0]    = colors[f][0];
                vert.color[1]    = colors[f][1];
                vert.color[2]    = colors[f][2];
                vert.normal[0]   = normals[f][0];
                vert.normal[1]   = normals[f][1];
                vert.normal[2]   = normals[f][2];
                vert.uv[0]       = uvs[v][0];
                vert.uv[1]       = uvs[v][1];
                data.vertices.push_back(vert);
            }
            data.indices.insert(data.indices.end(), {base, base + 1, base + 2, base + 2, base + 3, base});
        }
        return data;
    }

    GeometryData plane(const float width, const float height, const int subdivisionsX, const int subdivisionsZ) {
        GeometryData data;
        const float  stepX   = width / static_cast<float>(subdivisionsX);
        const float  stepZ   = height / static_cast<float>(subdivisionsZ);
        const float  originX = -width * 0.5f;
        const float  originZ = -height * 0.5f;

        for (int z = 0; z <= subdivisionsZ; ++z) {
            for (int x = 0; x <= subdivisionsX; ++x) {
                Vertex v{};
                v.position[0] = originX + x * stepX;
                v.position[1] = 0.0f;
                v.position[2] = originZ + z * stepZ;
                v.color[0] = v.color[1] = v.color[2] = 0.7f;
                v.normal[0]                          = 0.0f;
                v.normal[1]                          = 1.0f;
                v.normal[2]                          = 0.0f;
                v.uv[0]                              = static_cast<float>(x) / subdivisionsX;
                v.uv[1]                              = static_cast<float>(z) / subdivisionsZ;
                data.vertices.push_back(v);
            }
        }
        for (int z = 0; z < subdivisionsZ; ++z) {
            for (int x = 0; x < subdivisionsX; ++x) {
                const uint32_t i = z * (subdivisionsX + 1) + x;
                data.indices.insert(data.indices.end(), {i, i + 1, i + static_cast<uint32_t>(subdivisionsX) + 1, i + 1,
                                                         i + static_cast<uint32_t>(subdivisionsX) + 2,
                                                         i + static_cast<uint32_t>(subdivisionsX) + 1});
            }
        }
        return data;
    }
} // namespace Positron::Geometry
