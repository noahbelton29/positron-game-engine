//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "example_game.h"

#include <positron/core/log.h>

#include "positron/core/application.h"
#include "positron/core/input.h"

void ExampleGame::onInit() { POSITRON_INFO("ExampleGame initialized"); }

void ExampleGame::onUpdate() {
    if (Positron::Input::isKeyReleased(Positron::Key::F)) {
        POSITRON_INFO("F was pressed");
    }

    if (Positron::Input::isKeyReleased(Positron::Key::Escape)) {
        POSITRON_INFO("Closing game");
        quit();
    }
}

void ExampleGame::onShutdown() { POSITRON_INFO("Game shut down"); }

Positron::RenderAPI ExampleGame::getRenderAPI() const { return Positron::RenderAPI::OpenGL; }
