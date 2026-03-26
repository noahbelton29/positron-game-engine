//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "prefabs/bouncing_cube_prefab.h"
#include "components/bouncer_component.h"

BouncingCubePrefab::BouncingCubePrefab(const float amplitude, const float frequency, const float size) :
    bounceAmplitude_(amplitude), bounceFrequency_(frequency), cubeSize_(size) {}

std::string BouncingCubePrefab::name() const { return "BouncingCube"; }

Positron::MeshDesc BouncingCubePrefab::mesh() const { return Positron::MeshDesc::cube(cubeSize_); }

void BouncingCubePrefab::onCreate(Positron::EntityBuilder &builder) {
    builder.with<BouncerComponent>({bounceAmplitude_, bounceFrequency_});
}
