//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "example_game.h"

#include <positron/core/input.h>
#include <positron/core/log.h>
#include <positron/renderer/mesh_factory.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void ExampleGame::onInit() {
    POSITRON_INFO("ExampleGame initialized");

    cubeMesh_ = Positron::MeshFactory::cube(getRenderAPI());

    events_.subscribe<Positron::KeyPressedEvent>([this](const Positron::KeyPressedEvent &e) {
        if (e.getKey() == Positron::Key::Escape) {
            quit();
            return true;
        }
        return false;
    });
}

void ExampleGame::onUpdate() {
    rotation_ += 0.005f;

    const glm::mat4 transform = glm::rotate(glm::mat4(1.0f), rotation_, glm::vec3(0.0f, 1.0f, 0.0f)) *
                                glm::rotate(glm::mat4(1.0f), rotation_ * 0.5f, glm::vec3(1.0f, 0.0f, 0.0f));

    if (auto *renderer = getRenderer())
        renderer->drawMesh(cubeMesh_.get(), transform);
}

void ExampleGame::onShutdown() {
    POSITRON_INFO("Game shut down");
    cubeMesh_.reset();
}

void ExampleGame::onEvent(Positron::Event &e) {
    Positron::EventDispatcher d(e);

    d.dispatch<Positron::WindowResizeEvent>([](const Positron::WindowResizeEvent &ev) {
        POSITRON_INFO("Window resized: {}x{}", ev.getWidth(), ev.getHeight());
        return false;
    });
}

Positron::RenderAPI ExampleGame::getRenderAPI() const { return Positron::RenderAPI::OpenGL; }
