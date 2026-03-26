//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "mesh_type.h"
#include "positron/core/export.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/entity.h"
#include "positron/ecs/core/world.h"
#include "positron/renderer/geometry.h"

#include <glm/glm.hpp>
#include <string>

namespace Positron {
    class POSITRON_API EntityBuilder {
    public:
        EntityBuilder(World &world, Entity entity);

        template<typename TComponent>
        EntityBuilder &with(TComponent component = {}) {
            world_.addComponent<TComponent>(entity_, std::move(component));
            return *this;
        }

        [[nodiscard]] Entity entity() const { return entity_; }
        [[nodiscard]] World &world() const { return world_; }

    private:
        World &world_;
        Entity entity_;
    };

    struct POSITRON_API SpawnParams {
        glm::vec3 position = {0.f, 0.f, 0.f};
        glm::vec3 rotation = {0.f, 0.f, 0.f};
        glm::vec3 scale    = {1.f, 1.f, 1.f};

        SpawnParams &at(const glm::vec3 pos) {
            position = pos;
            return *this;
        }
        SpawnParams &rotated(const glm::vec3 euler) {
            rotation = euler;
            return *this;
        }
        SpawnParams &scaledTo(const glm::vec3 size) {
            scale = size;
            return *this;
        }
        SpawnParams &scaledTo(float uniform) {
            scale = {uniform, uniform, uniform};
            return *this;
        }
    };

    struct POSITRON_API MeshDesc {
        MeshType     type   = MeshType::Cube;
        float        size   = 1.f;
        float        planeW = 1.f;
        float        planeH = 1.f;
        int          subX   = 1;
        int          subZ   = 1;
        GeometryData geometry;

        static MeshDesc cube(float size = 1.f) { return {MeshType::Cube, size}; }

        static MeshDesc plane(float width, float height, int subdivisionsX = 1, int subdivisionsZ = 1) {
            return {MeshType::Plane, 1.f, width, height, subdivisionsX, subdivisionsZ};
        }

        static MeshDesc custom(GeometryData geo) {
            MeshDesc desc;
            desc.type     = MeshType::Custom;
            desc.geometry = std::move(geo);
            return desc;
        }
    };

    class POSITRON_API Prefab {
    public:
        virtual ~Prefab() = default;

        [[nodiscard]] virtual std::string name() const = 0;
        [[nodiscard]] virtual MeshDesc    mesh() const { return MeshDesc::cube(); }

        virtual void onCreate(EntityBuilder &builder) {}
    };
} // namespace Positron
