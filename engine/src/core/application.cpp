//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/core/application.h"

#include <iostream>

namespace Positron {
    Application::Application(ApplicationConfig config, std::unique_ptr<Game> game) :
        window_(std::make_unique<Window>(std::move(config.window))), game_(std::move(game)) {}

    void Application::run() const {
        if (!window_->init()) {
            std::cerr << "Failed to initialize window\n";
            return;
        }

        game_->onInit();

        while (!window_->shouldClose()) {
            game_->onUpdate();
            window_->onUpdate();
        }

        game_->onShutdown();
    }
} // namespace Positron
