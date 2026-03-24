//
// Copyright (c) 2025 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include <positron/core/window.h>

#include <iostream>

int main() {
    Positron::Window window(Positron::WindowConfig("Example Game"));

    if (!window.init()) {
        std::cerr << "Failed to initialize window" << std::endl;
        return -1;
    }

    while (!window.shouldClose()) {
        window.onUpdate();
    }

    return 0;
}
