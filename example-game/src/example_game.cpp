//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
//

#include "example_game.h"

#include <positron/core/input.h>
#include <positron/core/log.h>
#include <positron/renderer/shader.h>

void ExampleGame::onInit() {
    POSITRON_INFO("ExampleGame initialized");

    myShader_ = std::make_unique<Positron::Shader>("engine/assets/shaders/opengl/default.vert",
                                                   "engine/assets/shaders/opengl/new.frag");
    if (auto *renderer = getRenderer())
        renderer->setShader(myShader_.get());

    events_.subscribe<Positron::MouseScrolledEvent>([](Positron::MouseScrolledEvent &e) {
        POSITRON_INFO("Scroll: {:.2f}, {:.2f}", e.getXOffset(), e.getYOffset());
        return false;
    });

    events_.subscribe<EnemyDiedEvent>([](EnemyDiedEvent &e) {
        POSITRON_INFO("Enemy {} died", e.enemyId);
        return false;
    });

    events_.subscribe<PlayerDiedEvent>([](PlayerDiedEvent &) {
        POSITRON_INFO("Player died");
        return false;
    });

    Positron::EventBus::emit<EnemyDiedEvent>(42);
    Positron::EventBus::emit<PlayerDiedEvent>();
}

void ExampleGame::onEvent(Positron::Event &e) {
    Positron::EventDispatcher d(e);

    d.dispatch<Positron::KeyPressedEvent>([this](const Positron::KeyPressedEvent &ev) {
        if (ev.getKey() == Positron::Key::Escape) {
            POSITRON_INFO("Closing game");
            quit();
            return true;
        }
        if (ev.getKey() == Positron::Key::F) {
            POSITRON_INFO("F was pressed");
            return true;
        }
        return false;
    });

    d.dispatch<Positron::WindowResizeEvent>([](const Positron::WindowResizeEvent &ev) {
        POSITRON_INFO("Window resized: {}x{}", ev.getWidth(), ev.getHeight());
        return false;
    });

    d.dispatch<PlayerDiedEvent>([](PlayerDiedEvent &) {
        POSITRON_INFO("Player died (caught in onEvent)");
        return false;
    });
}

void ExampleGame::onUpdate() {
    if (Positron::Input::isKeyDown(Positron::Key::W))
        POSITRON_INFO("W held");
}

void ExampleGame::onShutdown() {
    POSITRON_INFO("Game shut down");

    if (auto *renderer = getRenderer())
        renderer->setShader(nullptr);

    myShader_.reset();
}

Positron::RenderAPI ExampleGame::getRenderAPI() const { return Positron::RenderAPI::OpenGL; }
