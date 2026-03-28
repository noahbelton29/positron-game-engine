//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 28/03/2026.
//

#include "prefabs/physics_cube_prefab.h"

#include <positron/ecs/component/components.h>

#include <utility>

#include "positron/core/log.h"
#include "positron/ecs/systems/audio_system.h"

PhysicsCubePrefab::PhysicsCubePrefab(const float size, const float mass, const float restitution, const float friction,
                                     std::string texturePath, const Positron::FilterMode filter) :
    cubeSize_(size), mass_(mass), restitution_(restitution), friction_(friction), texturePath_(std::move(texturePath)),
    filter_(filter) {}

std::string PhysicsCubePrefab::name() const { return "PhysicsCube"; }

Positron::MeshDesc PhysicsCubePrefab::mesh() const { return Positron::MeshDesc::cube(cubeSize_); }

void PhysicsCubePrefab::onCreate(Positron::EntityBuilder &builder) {
    Positron::BoxColliderComponent col;
    col.halfExtents = {cubeSize_ * 0.5f, cubeSize_ * 0.5f, cubeSize_ * 0.5f};
    builder.with<Positron::BoxColliderComponent>(col);

    Positron::RigidbodyComponent rb;
    rb.mass        = mass_;
    rb.restitution = restitution_;
    rb.friction    = friction_;
    builder.with<Positron::RigidbodyComponent>(rb);

    if (!texturePath_.empty())
        builder.with<Positron::TextureComponent>({texturePath_, filter_});

    Positron::AudioSourceComponent src;
    src.path            = "example-game/assets/sounds/bomb2.mp3";
    src.rolloffDistance = 2.f;
    src.maxDistance     = 5.f;
    src.loop            = false;
    src.playOnAwake     = false;
    builder.with<Positron::AudioSourceComponent>(src);

    Positron::CollisionCallbackComponent cb;
    cb.onContactAdded = [&world = builder.world(), self = builder.entity()](const Positron::CollisionInfo &info) {
        if (world.hasComponent<Positron::TagComponent>(info.other)) {
            const auto &[name] = world.getComponent<Positron::TagComponent>(info.other);
            POSITRON_INFO("PhysicsCube hit '{}'", name);
        }
        auto *audio = world.getSystem<Positron::AudioSystem>();
        audio->play(world, self); // play on self, not info.other
    };
    builder.with<Positron::CollisionCallbackComponent>(cb);
}
