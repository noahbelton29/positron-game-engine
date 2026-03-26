//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"

namespace Positron {
    class World;

    class POSITRON_API System {
    public:
        virtual ~System() = default;
        virtual void onInit(World &) {}
        virtual void onUpdate(World &world) = 0;
        virtual void onShutdown(World &) {}
    };
} // namespace Positron
