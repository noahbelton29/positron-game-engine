//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 28/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/ecs/core/system.h"

#include <memory>

namespace Positron {
    class POSITRON_API AudioSystem : public System {
    public:
        AudioSystem();
        ~AudioSystem() override;

        AudioSystem(const AudioSystem &)            = delete;
        AudioSystem &operator=(const AudioSystem &) = delete;

        void onInit(World &world) override;
        void onUpdate(World &world) override;
        void onShutdown(World &world) override;

        void play(World &world, Entity entity) const;
        void stop(World &world, Entity entity) const;

    private:
        struct Impl;
        std::unique_ptr<Impl> pImpl_;
    };

} // namespace Positron
