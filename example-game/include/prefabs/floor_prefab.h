//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include <positron/ecs/scene/prefab.h>

class FloorPrefab : public Positron::Prefab {
public:
    explicit FloorPrefab(float width = 16.f, float height = 16.f);

    [[nodiscard]] std::string        name() const override;
    [[nodiscard]] Positron::MeshDesc mesh() const override;

private:
    float floorWidth_;
    float floorHeight_;
};
