//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

namespace Positron {
    class Game {
    public:
        virtual ~Game() = default;

        virtual void onInit()     = 0;
        virtual void onUpdate()   = 0;
        virtual void onShutdown() = 0;
    };
} // namespace Positron
