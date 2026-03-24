//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "example_game.h"

#include <positron/core/application.h>

int main() {
    const Positron::Application app({.window = Positron::WindowConfig("My Game", 1280, 720)},
                                    std::make_unique<ExampleGame>());
    app.run();
}
