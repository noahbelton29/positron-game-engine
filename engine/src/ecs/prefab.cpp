//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/ecs/scene/prefab.h"

namespace Positron {
    EntityBuilder::EntityBuilder(World &world, const Entity entity) : world_(world), entity_(entity) {}
} // namespace Positron
