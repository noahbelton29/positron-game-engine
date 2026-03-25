//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/core/application.h"
#include "positron/core/event_bus.h"

#include <iostream>

#include "positron/core/input.h"
#include "positron/core/log.h"

namespace Positron {
    Application::Application(ApplicationConfig config, std::unique_ptr<Game> game) :
        window_(std::make_unique<Window>(std::move(config.window))), game_(std::move(game)),
        renderer_(Renderer::create(game_->getRenderAPI())) {
        Log::init();
    }

    void Application::run() {
        if (!window_->init()) {
            std::cerr << "Failed to initialize window\n";
            return;
        }

        Input::init(window_->getHandle());

        if (!renderer_->init()) {
            std::cerr << "Failed to initialize renderer\n";
            return;
        }

        game_->window_   = window_.get();
        game_->renderer_ = renderer_.get();

        EventBus::subscribe<WindowCloseEvent>([this](WindowCloseEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<WindowResizeEvent>([this](WindowResizeEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<KeyPressedEvent>([this](KeyPressedEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<KeyReleasedEvent>([this](KeyReleasedEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<MouseButtonPressedEvent>([this](MouseButtonPressedEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<MouseMovedEvent>([this](MouseMovedEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });
        EventBus::subscribe<MouseScrolledEvent>([this](MouseScrolledEvent &e) {
            game_->onEvent(e);
            return e.handled;
        });

        game_->onInit();

        while (running_ && !window_->shouldClose()) {
            Input::update();
            renderer_->begin();
            game_->onUpdate();
            renderer_->end();
            window_->onUpdate();

            if (game_->shouldQuit())
                running_ = false;
        }

        game_->onShutdown();
        renderer_->shutdown();
        EventBus::clear();
    }
} // namespace Positron
