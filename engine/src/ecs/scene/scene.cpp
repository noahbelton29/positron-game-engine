//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/ecs/scene/scene.h"
#include "positron/core/log.h"
#include "positron/ecs/systems/render_system.h"
#include "positron/renderer/mesh_factory.h"
#include "positron/renderer/texture.h"

namespace Positron {
    namespace {
        std::unique_ptr<Mesh> buildMesh(const RenderAPI api, const MeshDesc &desc) {
            switch (desc.type) {
                case MeshType::Cube:
                    return MeshFactory::cube(api, desc.size);
                case MeshType::Plane:
                    return MeshFactory::plane(api, desc.planeW, desc.planeH, desc.subX, desc.subZ);
                case MeshType::Custom:
                    return MeshFactory::create(api, desc.geometry);
                default:
                    return nullptr;
            }
        }
    } // namespace

    Scene::Scene(Renderer *renderer) : renderer_(renderer) { world_.setResource<Renderer *>(renderer_); }

    Entity Scene::spawnPrefab(Prefab &prefab, const SpawnParams &params) {
        const Entity entity = world_.createEntity();
        world_.addComponent<TagComponent>(entity, TagComponent{prefab.name()});
        world_.addComponent<TransformComponent>(entity,
                                                TransformComponent{params.position, params.rotation, params.scale});

        const MeshDesc &desc = prefab.mesh();
        if (auto mesh = buildMesh(renderer_->getAPI(), desc)) {
            Mesh *raw = storeMesh(std::move(mesh));
            world_.addComponent<MeshRendererComponent>(entity, MeshRendererComponent{raw, {}, true});
            world_.addComponent<MeshDescComponent>(
                    entity, MeshDescComponent{desc.type, desc.size, desc.planeW, desc.planeH, desc.subX, desc.subZ});
        }

        EntityBuilder builder(world_, entity);
        prefab.onCreate(builder);

        if (world_.hasComponent<TextureComponent>(entity) && world_.hasComponent<MeshRendererComponent>(entity)) {
            auto &[path, filter] = world_.getComponent<TextureComponent>(entity);
            auto *tex            = storeTexture(Texture::load(path, filter));
            world_.getComponent<MeshRendererComponent>(entity).material.albedo = tex;
        }

        return entity;
    }

    Entity Scene::spawnFromDesc(const std::string &name, const MeshDesc &desc, const SpawnParams &params) {
        const Entity entity = world_.createEntity();
        world_.addComponent<TagComponent>(entity, TagComponent{name});
        world_.addComponent<TransformComponent>(entity,
                                                TransformComponent{params.position, params.rotation, params.scale});

        if (auto mesh = buildMesh(renderer_->getAPI(), desc)) {
            Mesh *raw = storeMesh(std::move(mesh));
            world_.addComponent<MeshRendererComponent>(entity, MeshRendererComponent{raw, {}, true});
            world_.addComponent<MeshDescComponent>(
                    entity, MeshDescComponent{desc.type, desc.size, desc.planeW, desc.planeH, desc.subX, desc.subZ});
        }

        return entity;
    }

    Entity Scene::spawnEmpty(const std::string &name) {
        const Entity entity = world_.createEntity();
        world_.addComponent<TagComponent>(entity, TagComponent{name});
        return entity;
    }

    void Scene::destroy(const Entity entity) { world_.destroyEntity(entity); }

    Entity Scene::findByName(const std::string &name) {
        Entity found = NullEntity;
        world_.each<TagComponent>([&](const Entity entity, const TagComponent &tag) {
            if (found == NullEntity && tag.name == name)
                found = entity;
        });
        return found;
    }

    void Scene::attachMesh(const Entity entity, const MeshDesc &desc) {
        if (auto mesh = buildMesh(renderer_->getAPI(), desc)) {
            Mesh *raw = storeMesh(std::move(mesh));
            if (world_.hasComponent<MeshRendererComponent>(entity))
                world_.getComponent<MeshRendererComponent>(entity).mesh = raw;
            else
                world_.addComponent<MeshRendererComponent>(entity, MeshRendererComponent{raw, {}, true});
            const MeshDescComponent mdc{desc.type, desc.size, desc.planeW, desc.planeH, desc.subX, desc.subZ};
            if (world_.hasComponent<MeshDescComponent>(entity))
                world_.getComponent<MeshDescComponent>(entity) = mdc;
            else
                world_.addComponent<MeshDescComponent>(entity, mdc);
        }
    }

    void Scene::applyTexture(const Entity entity) {
        if (!world_.hasComponent<TextureComponent>(entity) || !world_.hasComponent<MeshRendererComponent>(entity))
            return;
        auto &[path, filter] = world_.getComponent<TextureComponent>(entity);
        auto *tex            = storeTexture(Texture::load(path, filter));
        world_.getComponent<MeshRendererComponent>(entity).material.albedo = tex;
    }


    World       &Scene::world() { return world_; }
    const World &Scene::world() const { return world_; }

    void Scene::update() {
        world_.update();
        renderSystem_.onUpdate(world_);
    }

    void Scene::shutdown() {
        world_.shutdown();
        renderSystem_.onShutdown(world_);
    }

    Mesh *Scene::storeMesh(std::unique_ptr<Mesh> mesh) {
        Mesh *raw = mesh.get();
        meshes_.push_back(std::move(mesh));
        return raw;
    }

    Texture *Scene::storeTexture(Texture *texture) {
        textures_.emplace_back(texture);
        return texture;
    }

} // namespace Positron
