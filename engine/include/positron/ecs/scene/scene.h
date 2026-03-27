//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/entity.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/systems/render_system.h"
#include "positron/renderer/mesh.h"
#include "positron/renderer/renderer.h"
#include "prefab.h"

#include <memory>
#include <string>
#include <vector>

namespace Positron {
    class POSITRON_API Scene {
    public:
        explicit Scene(Renderer *renderer);
        ~Scene() = default;

        Scene(const Scene &)            = delete;
        Scene &operator=(const Scene &) = delete;

        template<typename T, typename... Args>
        Entity spawn(const SpawnParams params = {}, Args &&...args) {
            static_assert(std::is_base_of_v<Prefab, T>, "T must derive from Prefab");

            auto prefab = std::make_unique<T>(std::forward<Args>(args)...);
            return spawnPrefab(*prefab, params);
        }

        Entity spawnFromDesc(const std::string &name, const MeshDesc &desc, const SpawnParams &params = {});

        void attachMesh(Entity entity, const MeshDesc &desc);
        void applyTexture(Entity entity);

        Entity spawnEmpty(const std::string &name = "Entity");

        void destroy(Entity entity);

        Entity findByName(const std::string &name);

        World       &world();
        const World &world() const;

        template<typename T, typename... Args>
        T *addSystem(Args &&...args) {
            return world_.addSystem<T>(std::forward<Args>(args)...);
        }

        template<typename T>
        T *getSystem() {
            return world_.getSystem<T>();
        }

        void update();
        void shutdown();

    private:
        Entity   spawnPrefab(Prefab &prefab, const SpawnParams &params);
        Mesh    *storeMesh(std::unique_ptr<Mesh> mesh);
        Texture *storeTexture(Texture *texture);

        Renderer                             *renderer_;
        World                                 world_;
        RenderSystem                          renderSystem_;
        std::vector<std::unique_ptr<Mesh>>    meshes_;
        std::vector<std::unique_ptr<Texture>> textures_;
    };
} // namespace Positron
