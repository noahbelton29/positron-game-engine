//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/core/application.h"

#include <iostream>

#include "positron/core/log.h"

namespace Positron {
    Application::Application(ApplicationConfig config, std::unique_ptr<Game> game) :
        window_(std::make_unique<Window>(std::move(config.window))), game_(std::move(game)),
        renderer_(Renderer::create(game_->getRenderAPI())) {
        Log::init();
    }

    void Application::run() const {
        if (!window_->init()) {
            std::cerr << "Failed to initialize window\n";
            return;
        }

        if (!renderer_->init()) {
            std::cerr << "Failed to initialize renderer\n";
            return;
        }

        game_->onInit();

        while (!window_->shouldClose()) {
            renderer_->begin();
            game_->onUpdate();
            renderer_->end();
            window_->onUpdate();
        }

        game_->onShutdown();
        renderer_->shutdown();
    }
} // namespace Positron
