//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "example_game.h"

#include <positron/core/input.h>
#include <positron/core/log.h>
#include <positron/ecs/scene/scene_serializer.h>

#include "components/bouncer_component.h"
#include "components/camera_component.h"
#include "components/spinner_component.h"

#include "prefabs/bouncing_cube_prefab.h"
#include "prefabs/floor_prefab.h"
#include "prefabs/physics_cube_prefab.h"
#include "prefabs/spinning_cube_prefab.h"

#include "systems/bounce_system.h"
#include "systems/camera_system.h"
#include "systems/spin_system.h"

#include <positron/ecs/systems/physics_system.h>

#include "positron/ecs/systems/audio_system.h"

static constexpr auto SAVE_PATH = "scene.pscn";

static Positron::SceneSerializer makeSerializer(Positron::Scene &scene) {
    Positron::SceneSerializer s(scene);
    s.autoExtend<CameraComponent>();
    s.autoExtend<SpinnerComponent>();
    s.autoExtend<BouncerComponent>();
    return s;
}

void ExampleGame::onInit() {
    POSITRON_INFO("ExampleGame initialised");

    Positron::Scene &scene = getScene();

    scene.addSystem<Positron::PhysicsSystem>();
    scene.addSystem<CameraSystem>();
    scene.addSystem<SpinSystem>();
    scene.addSystem<BounceSystem>();
    scene.addSystem<Positron::AudioSystem>();

    const auto cam = scene.spawnEmpty("Camera");
    scene.world().addComponent<Positron::TransformComponent>(cam,
                                                             Positron::TransformComponent{.position = {0.f, 0.f, 3.f}});
    scene.world().addComponent<CameraComponent>(cam);
    scene.world().addComponent<Positron::AudioListenerComponent>(cam);

    const auto sun = scene.spawnEmpty("Sun");
    scene.world().addComponent<Positron::TransformComponent>(
            sun, {.position = {0.f, 5.f, 3.f}, .rotation = {-45.f, 0.f, 0.f}});
    scene.world().addComponent<Positron::LightComponent>(sun, {.type        = Positron::LightType::Directional,
                                                               .color       = {1.f, 1.0f, 1.0f},
                                                               .intensity   = 1.2f,
                                                               .castShadows = true});

    {
        const auto e = scene.spawn<PhysicsCubePrefab>(Positron::SpawnParams{}.at({-3.f, 1.f, 0.f}));
        scene.world().getComponent<Positron::RigidbodyComponent>(e).linearVelocity = {1.5f, 0.f, 0.f};
    }
    {
        const auto e = scene.spawn<PhysicsCubePrefab>(Positron::SpawnParams{}.at({3.f, 1.15f, 0.f}));
        scene.world().getComponent<Positron::RigidbodyComponent>(e).linearVelocity = {-11.f, 0.f, 0.f};
    }

    scene.spawn<FloorPrefab>(Positron::SpawnParams{}.at({0.f, -1.f, 0.f}));

    events_.subscribe<Positron::KeyPressedEvent>([this, &scene](const Positron::KeyPressedEvent &keyEvent) {
        if (keyEvent.getKey() == Positron::Key::Escape) {
            quit();
            return true;
        }

        if (keyEvent.getKey() == Positron::Key::F5) {
            if (const auto serializer = makeSerializer(scene); serializer.save(SAVE_PATH))
                POSITRON_INFO("Scene saved to {}", SAVE_PATH);
            return true;
        }

        if (keyEvent.getKey() == Positron::Key::F9) {
            if (const auto serializer = makeSerializer(scene); serializer.load(SAVE_PATH))
                POSITRON_INFO("Scene loaded from {}", SAVE_PATH);
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
