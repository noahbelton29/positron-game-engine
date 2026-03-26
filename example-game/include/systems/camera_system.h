//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include <positron/ecs/core/system.h>
#include <positron/ecs/scene/camera_data.h>

class CameraSystem final : public Positron::System {
public:
    float moveSpeed = 5.f;
    float lookSpeed = 0.1f;

    void onInit(Positron::World &world) override;
    void onUpdate(Positron::World &world) override;

private:
    double lastMouseX_ = 0.0;
    double lastMouseY_ = 0.0;
    bool   firstFrame_ = true;
};
