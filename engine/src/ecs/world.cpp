//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/ecs/core/world.h"

#include <ranges>

namespace Positron {
    Entity World::createEntity() {
        const Entity entity = nextEntity_++;
        alive_.insert(entity);
        return entity;
    }

    void World::destroyEntity(const Entity entity) {
        if (!alive_.contains(entity))
            return;
        for (const auto &stor: storages_ | std::views::values)
            stor->remove(entity);
        alive_.erase(entity);
    }

    bool World::isAlive(const Entity entity) const { return alive_.contains(entity); }

    void World::update() {
        for (const auto &s: systems_)
            s->onUpdate(*this);
    }

    void World::shutdown() {
        for (const auto &s: systems_)
            s->onShutdown(*this);
        systems_.clear();
    }
} // namespace Positron
