//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include <positron/ecs/component/components.h>
#include <positron/ecs/core/world.h>

#include "components/spinner_component.h"
#include "systems/spin_system.h"

void SpinSystem::onUpdate(Positron::World &world) {
    world.each<Positron::TransformComponent, SpinnerComponent>(
            [](Positron::Entity, Positron::TransformComponent &transform, const SpinnerComponent &spinner) {
                transform.rotation.y += spinner.speed;
                transform.rotation.x += spinner.speed * 0.5f;
            });
}
