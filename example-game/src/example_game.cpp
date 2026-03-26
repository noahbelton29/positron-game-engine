//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "example_game.h"

#include <positron/core/input.h>
#include <positron/core/log.h>

#include "components/camera_component.h"

#include "prefabs/bouncing_cube_prefab.h"
#include "prefabs/floor_prefab.h"
#include "prefabs/spinning_cube_prefab.h"

#include "systems/bounce_system.h"
#include "systems/camera_system.h"
#include "systems/spin_system.h"

void ExampleGame::onInit() {
    POSITRON_INFO("ExampleGame initialised");

    Positron::Scene &scene = getScene();

    scene.addSystem<CameraSystem>();
    scene.addSystem<SpinSystem>();
    scene.addSystem<BounceSystem>();

    const auto cam = scene.spawnEmpty("Camera");
    scene.world().addComponent<Positron::TransformComponent>(cam, Positron::TransformComponent{{0.f, 0.f, 3.f}});
    scene.world().addComponent<CameraComponent>(cam);

    const auto sun = scene.spawnEmpty("Sun");
    scene.world().addComponent<Positron::TransformComponent>(sun, {{0.f, 5.f, 3.f}, {-45.f, 0.f, 0.f}});
    scene.world().addComponent<Positron::LightComponent>(sun, {.type        = Positron::LightType::Directional,
                                                               .color       = {1.f, 1.0f, 1.0f},
                                                               .intensity   = 1.2f,
                                                               .castShadows = true});

    scene.spawn<SpinningCubePrefab>(Positron::SpawnParams{}.at({-1.5f, 0.f, 0.f}));
    scene.spawn<BouncingCubePrefab>(Positron::SpawnParams{}.at({1.5f, 0.f, 0.f}));
    scene.spawn<FloorPrefab>(Positron::SpawnParams{}.at({0.f, -1.f, 0.f}));

    events_.subscribe<Positron::KeyPressedEvent>([this, &scene](const Positron::KeyPressedEvent &keyEvent) {
        if (keyEvent.getKey() == Positron::Key::Escape) {
            quit();
            return true;
        }
        if (keyEvent.getKey() == Positron::Key::Space) {
            static float spawnOffsetX = 0.f;
            spawnOffsetX += 0.5f;
            scene.spawn<SpinningCubePrefab>(Positron::SpawnParams{}.at({spawnOffsetX, 0.f, -3.f}), 2.f, 0.5f);
            POSITRON_INFO("Spawned spinning cube at x={}", spawnOffsetX);

            EnemyDiedEvent enemyEvent(42);
            Positron::EventBus::emit(enemyEvent);
        }
        return false;
    });

    events_.subscribe<EnemyDiedEvent>([](const EnemyDiedEvent &enemyEvent) {
        POSITRON_INFO("Enemy {} died!", enemyEvent.enemyId);
        return false;
    });
}

void ExampleGame::onUpdate() {}

void ExampleGame::onShutdown() { POSITRON_INFO("ExampleGame shutdown"); }

void ExampleGame::onEvent(Positron::Event &event) {
    Positron::EventDispatcher dispatcher(event);
    dispatcher.dispatch<Positron::WindowResizeEvent>([](const Positron::WindowResizeEvent &resizeEvent) {
        POSITRON_INFO("Resized: {}x{}", resizeEvent.getWidth(), resizeEvent.getHeight());
        return false;
    });
}

Positron::RenderAPI ExampleGame::getRenderAPI() const { return Positron::RenderAPI::OpenGL; }
