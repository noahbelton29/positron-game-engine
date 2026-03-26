//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include <glm/glm.hpp>

struct CameraData {
    glm::mat4 view       = glm::mat4(1.f);
    glm::mat4 projection = glm::mat4(1.f);
};
