//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/ecs/component/components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Positron {
    glm::mat4 TransformComponent::matrix() const {
        glm::mat4 m = glm::translate(glm::mat4(1.f), position);
        m           = glm::rotate(m, glm::radians(rotation.y), {0.f, 1.f, 0.f});
        m           = glm::rotate(m, glm::radians(rotation.x), {1.f, 0.f, 0.f});
        m           = glm::rotate(m, glm::radians(rotation.z), {0.f, 0.f, 1.f});
        m           = glm::scale(m, scale);
        return m;
    }
} // namespace Positron
