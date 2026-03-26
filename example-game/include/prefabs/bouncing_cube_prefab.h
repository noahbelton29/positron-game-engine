//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include <positron/ecs/scene/prefab.h>

class BouncingCubePrefab : public Positron::Prefab {
public:
    explicit BouncingCubePrefab(float amplitude = 0.6f, float frequency = 2.f, float size = 0.8f);

    [[nodiscard]] std::string        name() const override;
    [[nodiscard]] Positron::MeshDesc mesh() const override;
    void                             onCreate(Positron::EntityBuilder &builder) override;

private:
    float bounceAmplitude_;
    float bounceFrequency_;
    float cubeSize_;
};
