//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include "positron/core/game.h"
#include "positron/core/window.h"
#include "positron/renderer/renderer.h"

#include <memory>

namespace Positron {
    struct ApplicationConfig {
        WindowConfig window;
    };

    class Application {
    public:
        Application(ApplicationConfig config, std::unique_ptr<Game> game);
        ~Application() = default;

        Application(const Application &)            = delete;
        Application &operator=(const Application &) = delete;

        void run() const;

    private:
        std::unique_ptr<Window>   window_;
        std::unique_ptr<Game>     game_;
        std::unique_ptr<Renderer> renderer_;
    };
} // namespace Positron
