//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include <positron/ecs/core/system.h>

class SpinSystem : public Positron::System {
public:
    void onUpdate(Positron::World &world) override;
};
