//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include <positron/ecs/component/components.h>
#include <positron/ecs/core/world.h>

#include <cmath>

#include "components/bouncer_component.h"
#include "systems/bounce_system.h"

void BounceSystem::onUpdate(Positron::World &world) {
    const float dt = world.getResource<float>();
    world.each<Positron::TransformComponent, BouncerComponent>(
            [dt](Positron::Entity, Positron::TransformComponent &transform, BouncerComponent &bouncer) {
                bouncer.time += dt;
                transform.position.y = std::sin(bouncer.time * bouncer.frequency) * bouncer.amplitude;
            });
}
