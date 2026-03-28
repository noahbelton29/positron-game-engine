//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 28/03/2026.
//

#pragma once

#include <positron/ecs/scene/prefab.h>
#include <positron/renderer/texture.h>

#include <string>

class PhysicsCubePrefab : public Positron::Prefab {
public:
    explicit PhysicsCubePrefab(float size = 0.5f, float mass = 1.f, float restitution = 0.4f, float friction = 0.6f,
                               std::string          texturePath = "example-game/assets/textures/crate.png",
                               Positron::FilterMode filter      = Positron::FilterMode::Nearest);

    [[nodiscard]] std::string        name() const override;
    [[nodiscard]] Positron::MeshDesc mesh() const override;
    void                             onCreate(Positron::EntityBuilder &builder) override;

private:
    float                cubeSize_;
    float                mass_;
    float                restitution_;
    float                friction_;
    std::string          texturePath_;
    Positron::FilterMode filter_;
};
