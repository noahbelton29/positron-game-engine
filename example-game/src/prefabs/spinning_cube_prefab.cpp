//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include <positron/ecs/component/components.h>
#include <positron/ecs/core/world.h>

#include <utility>

#include "components/spinner_component.h"
#include "prefabs/spinning_cube_prefab.h"

SpinningCubePrefab::SpinningCubePrefab(const float speed, const float size, std::string texturePath,
                                       const Positron::FilterMode filter) :
    spinSpeed_(speed), cubeSize_(size), texturePath_(std::move(texturePath)), filter_(filter) {}

std::string SpinningCubePrefab::name() const { return "SpinningCube"; }

Positron::MeshDesc SpinningCubePrefab::mesh() const { return Positron::MeshDesc::cube(cubeSize_); }

void SpinningCubePrefab::onCreate(Positron::EntityBuilder &builder) {
    builder.with<SpinnerComponent>({spinSpeed_});

    if (!texturePath_.empty()) {
        builder.with<Positron::TextureComponent>({texturePath_, filter_});
    }
}
