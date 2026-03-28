//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 28/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/ecs/core/entity.h"
#include "positron/ecs/core/system.h"

#include <memory>

namespace JPH {
    class PhysicsSystem;
    class TempAllocatorImpl;
    class JobSystemThreadPool;
    class BroadPhaseLayerInterface;
    class ObjectVsBroadPhaseLayerFilter;
    class ObjectLayerPairFilter;
    class ContactListener;
} // namespace JPH

namespace Positron {
    namespace PhysicsLayer {
        static constexpr uint8_t NonMoving = 0;
        static constexpr uint8_t Moving    = 1;
        static constexpr uint8_t NumLayers = 2;
    } // namespace PhysicsLayer

    class POSITRON_API PhysicsSystem : public System {
    public:
        PhysicsSystem();
        ~PhysicsSystem() override;

        PhysicsSystem(const PhysicsSystem &)            = delete;
        PhysicsSystem &operator=(const PhysicsSystem &) = delete;

        void onInit(World &world) override;
        void onUpdate(World &world) override;
        void onShutdown(World &world) override;

        void setSubSteps(const int steps) { subSteps_ = steps; }
        void setGravity(float x, float y, float z) const;

    private:
        void registerOrUpdateBody(Entity entity, World &world) const;
        void registerMeshBody(Entity entity, World &world) const;

        int subSteps_ = 1;

        struct Impl;
        std::unique_ptr<Impl> pImpl_;
    };

} // namespace Positron
