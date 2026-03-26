//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

struct CameraComponent {
    float fov       = 60.f;
    float nearPlane = 0.1f;
    float farPlane  = 1000.f;
    bool  active    = true;
};
