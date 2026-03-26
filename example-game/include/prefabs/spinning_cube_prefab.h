//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include <positron/ecs/scene/prefab.h>
#include <positron/renderer/texture.h>

#include <string>

class SpinningCubePrefab : public Positron::Prefab {
public:
    explicit SpinningCubePrefab(float speed = 1.2f, float size = 0.8f,
                                std::string          texturePath = "example-game/assets/textures/crate.png",
                                Positron::FilterMode filter      = Positron::FilterMode::Nearest);

    [[nodiscard]] std::string        name() const override;
    [[nodiscard]] Positron::MeshDesc mesh() const override;
    void                             onCreate(Positron::EntityBuilder &builder) override;

private:
    float                spinSpeed_;
    float                cubeSize_;
    std::string          texturePath_;
    Positron::FilterMode filter_;
};
