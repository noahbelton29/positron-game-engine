//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/ecs/core/system.h"

namespace Positron {
    class POSITRON_API RenderSystem final : public System {
    public:
        void onUpdate(World &world) override;
    };
} // namespace Positron
