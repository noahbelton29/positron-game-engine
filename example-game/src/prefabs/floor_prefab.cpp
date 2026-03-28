//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "prefabs/floor_prefab.h"

#include <positron/ecs/component/components.h>
#include <positron/renderer/geometry.h>

FloorPrefab::FloorPrefab(const float width, const float height) : floorWidth_(width), floorHeight_(height) {}

std::string FloorPrefab::name() const { return "Floor"; }

Positron::MeshDesc FloorPrefab::mesh() const { return Positron::MeshDesc::plane(floorWidth_, floorHeight_, 4, 4); }

void FloorPrefab::onCreate(Positron::EntityBuilder &builder) {
    // Build a MeshColliderComponent whose geometry exactly matches the visual plane mesh.
    // This gives pixel-perfect collision at zero thickness — no slab approximation.
    Positron::MeshColliderComponent col;
    col.geometry = Positron::Geometry::plane(floorWidth_, floorHeight_, 4, 4);
    builder.with<Positron::MeshColliderComponent>(std::move(col));
}
