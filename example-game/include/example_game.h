//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include <positron/core/game.h>

class example_game : public Positron::Game {
public:
    void onInit() override;
    void onUpdate() override;
    void onShutdown() override;
};
