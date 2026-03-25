//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "example_game.h"

#include <iostream>

#include "positron/core/log.h"

void ExampleGame::onInit() {
    POSITRON_INFO("ExampleGame initialized");
    int *ptr = nullptr;
    *ptr     = 42;
}

void ExampleGame::onUpdate() {}

void ExampleGame::onShutdown() {}

Positron::RenderAPI ExampleGame::getRenderAPI() const { return Positron::RenderAPI::OpenGL; }
