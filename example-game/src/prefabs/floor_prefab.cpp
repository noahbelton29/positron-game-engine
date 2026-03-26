//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "prefabs/floor_prefab.h"

FloorPrefab::FloorPrefab(const float width, const float height) : floorWidth_(width), floorHeight_(height) {}

std::string FloorPrefab::name() const { return "Floor"; }

Positron::MeshDesc FloorPrefab::mesh() const { return Positron::MeshDesc::plane(floorWidth_, floorHeight_, 4, 4); }
