//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/renderer/mesh.h"
#include "positron/renderer/texture.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace Positron {
    struct POSITRON_API TransformComponent {
        glm::vec3 position = {0.f, 0.f, 0.f};
        glm::vec3 rotation = {0.f, 0.f, 0.f};
        glm::vec3 scale    = {1.f, 1.f, 1.f};

        [[nodiscard]] glm::mat4 matrix() const;
    };

    struct POSITRON_API MaterialComponent {
        Texture  *albedo = nullptr;
        glm::vec4 tint   = {1.f, 1.f, 1.f, 1.f};
        float     tiling = 1.f;
    };

    struct POSITRON_API TextureComponent {
        std::string path;
        FilterMode  filter = FilterMode::Linear;
    };

    struct POSITRON_API MeshRendererComponent {
        Mesh             *mesh     = nullptr;
        MaterialComponent material = {};
        bool              visible  = true;
    };

    struct POSITRON_API TagComponent {
        std::string name;
    };
} // namespace Positron
