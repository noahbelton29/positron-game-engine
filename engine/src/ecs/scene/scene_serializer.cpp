//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 27/03/2026.
//

#include "positron/ecs/scene/scene_serializer.h"
#include "positron/core/log.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/scene/prefab.h"
#include "positron/renderer/mesh_factory.h"
#include "positron/renderer/texture.h"

#include <cstring>
#include <fstream>
#include <string>
#include <vector>

namespace Positron {
    namespace {
        constexpr uint8_t PSCN_VERSION  = 1;
        constexpr char    PSCN_MAGIC[4] = {'P', 'S', 'C', 'N'};

        constexpr uint8_t COMP_TAG       = 1 << 0;
        constexpr uint8_t COMP_TRANSFORM = 1 << 1;
        constexpr uint8_t COMP_MESH      = 1 << 2;
        constexpr uint8_t COMP_LIGHT     = 1 << 3;
        constexpr uint8_t COMP_TEXTURE   = 1 << 4;

        template<typename T>
        void write(std::ofstream &f, const T &v) {
            f.write(reinterpret_cast<const char *>(&v), sizeof(T));
        }

        void writeStr(std::ofstream &f, const std::string &s) {
            const auto len = static_cast<uint16_t>(s.size());
            write(f, len);
            f.write(s.data(), len);
        }

        template<typename T>
        void read(std::ifstream &f, T &v) {
            f.read(reinterpret_cast<char *>(&v), sizeof(T));
        }

        std::string readStr(std::ifstream &f) {
            uint16_t len = 0;
            read(f, len);
            std::string s(len, '\0');
            f.read(s.data(), len);
            return s;
        }
    } // anonymous namespace

    SceneSerializer::SceneSerializer(Scene &scene) : scene_(scene) {}

    bool SceneSerializer::save(const std::string &path) const {
        std::ofstream f(path, std::ios::binary | std::ios::trunc);
        if (!f) {
            POSITRON_ERROR("SceneSerializer: cannot open '{}' for writing", path);
            return false;
        }

        f.write(PSCN_MAGIC, 4);
        write(f, PSCN_VERSION);

        const auto extCount = static_cast<uint8_t>(extensions_.size());
        write(f, extCount);

        World &world = scene_.world();

        std::vector<Entity> entities;
        world.each<TagComponent>([&](Entity e, TagComponent &) { entities.push_back(e); });

        const auto entityCount = static_cast<uint32_t>(entities.size());
        write(f, entityCount);

        for (Entity e: entities) {
            uint8_t mask = 0;
            if (world.hasComponent<TagComponent>(e))
                mask |= COMP_TAG;
            if (world.hasComponent<TransformComponent>(e))
                mask |= COMP_TRANSFORM;
            if (world.hasComponent<MeshRendererComponent>(e))
                mask |= COMP_MESH;
            if (world.hasComponent<LightComponent>(e))
                mask |= COMP_LIGHT;
            if (world.hasComponent<TextureComponent>(e))
                mask |= COMP_TEXTURE;
            write(f, mask);

            uint32_t extMask = 0;
            for (size_t i = 0; i < extensions_.size(); ++i)
                if (extensions_[i].has(world, e))
                    extMask |= (1u << i);
            write(f, extMask);

            if (mask & COMP_TAG)
                writeStr(f, world.getComponent<TagComponent>(e).name);

            if (mask & COMP_TRANSFORM) {
                const auto &[position, rotation, scale, physicsRotation] = world.getComponent<TransformComponent>(e);
                write(f, position.x);
                write(f, position.y);
                write(f, position.z);
                write(f, rotation.x);
                write(f, rotation.y);
                write(f, rotation.z);
                write(f, scale.x);
                write(f, scale.y);
                write(f, scale.z);
            }
            if (mask & COMP_MESH) {
                const auto &mr = world.getComponent<MeshRendererComponent>(e);
                write(f, mr.visible);
                write(f, mr.material.tint.r);
                write(f, mr.material.tint.g);
                write(f, mr.material.tint.b);
                write(f, mr.material.tint.a);
                write(f, mr.material.tiling);

                const bool hasMeshDesc = world.hasComponent<MeshDescComponent>(e);
                write(f, hasMeshDesc);
                if (hasMeshDesc) {
                    const auto &[type, size, planeW, planeH, subX, subZ] = world.getComponent<MeshDescComponent>(e);
                    write(f, static_cast<uint8_t>(type));
                    write(f, size);
                    write(f, planeW);
                    write(f, planeH);
                    write(f, static_cast<int32_t>(subX));
                    write(f, static_cast<int32_t>(subZ));
                }
            }

            if (mask & COMP_LIGHT) {
                const auto &[type, color, intensity, range, attenLinear, attenQuad, direction, innerAngle, outerAngle,
                             castShadows] = world.getComponent<LightComponent>(e);
                write(f, static_cast<uint8_t>(type));
                write(f, color.x);
                write(f, color.y);
                write(f, color.z);
                write(f, intensity);
                write(f, range);
                write(f, attenLinear);
                write(f, attenQuad);
                write(f, direction.x);
                write(f, direction.y);
                write(f, direction.z);
                write(f, innerAngle);
                write(f, outerAngle);
                write(f, castShadows);
            }

            if (mask & COMP_TEXTURE) {
                const auto &tc = world.getComponent<TextureComponent>(e);
                writeStr(f, tc.path);
                write(f, static_cast<uint8_t>(tc.filter));
            }

            for (size_t i = 0; i < extensions_.size(); ++i)
                if (extMask & (1u << i))
                    extensions_[i].save(f, world, e);
        }

        POSITRON_INFO("SceneSerializer: saved {} entities to '{}'", entityCount, path);
        return true;
    }

    bool SceneSerializer::load(const std::string &path) const {
        std::ifstream f(path, std::ios::binary);
        if (!f) {
            POSITRON_ERROR("SceneSerializer: cannot open '{}' for reading", path);
            return false;
        }

        char magic[4];
        f.read(magic, 4);
        if (std::memcmp(magic, PSCN_MAGIC, 4) != 0) {
            POSITRON_ERROR("SceneSerializer: '{}' is not a valid .pscn file", path);
            return false;
        }

        uint8_t version = 0;
        read(f, version);
        if (version != PSCN_VERSION) {
            POSITRON_ERROR("SceneSerializer: unsupported .pscn version {}", version);
            return false;
        }

        uint8_t savedExtCount = 0;
        read(f, savedExtCount);
        if (savedExtCount != static_cast<uint8_t>(extensions_.size())) {
            POSITRON_WARN("SceneSerializer: file has {} extensions, but {} are registered, custom components may not "
                          "load correctly",
                          savedExtCount, extensions_.size());
        }

        {
            World              &world = scene_.world();
            std::vector<Entity> toDestroy;
            world.each<TagComponent>([&](const Entity e, TagComponent &) { toDestroy.push_back(e); });
            for (Entity e: toDestroy)
                scene_.destroy(e);
        }

        uint32_t entityCount = 0;
        read(f, entityCount);

        for (uint32_t i = 0; i < entityCount; ++i) {
            uint8_t  mask    = 0;
            uint32_t extMask = 0;
            read(f, mask);
            read(f, extMask);

            Entity e     = scene_.spawnEmpty();
            World &world = scene_.world();

            if (mask & COMP_TAG)
                world.getComponent<TagComponent>(e).name = readStr(f);

            if (mask & COMP_TRANSFORM) {
                TransformComponent t;
                read(f, t.position.x);
                read(f, t.position.y);
                read(f, t.position.z);
                read(f, t.rotation.x);
                read(f, t.rotation.y);
                read(f, t.rotation.z);
                read(f, t.scale.x);
                read(f, t.scale.y);
                read(f, t.scale.z);
                world.addComponent<TransformComponent>(e, t);
            }

            if (mask & COMP_MESH) {
                bool  visible = true;
                float tr, tg, tb, ta, tiling;
                read(f, visible);
                read(f, tr);
                read(f, tg);
                read(f, tb);
                read(f, ta);
                read(f, tiling);

                bool hasMeshDesc = false;
                read(f, hasMeshDesc);
                if (hasMeshDesc) {
                    uint8_t meshTypeVal = 0;
                    float   meshSize    = 1.f;
                    float   planeW      = 1.f;
                    float   planeH      = 1.f;
                    int32_t subX        = 1;
                    int32_t subZ        = 1;
                    read(f, meshTypeVal);
                    read(f, meshSize);
                    read(f, planeW);
                    read(f, planeH);
                    read(f, subX);
                    read(f, subZ);

                    MeshDesc desc;
                    switch (static_cast<MeshType>(meshTypeVal)) {
                        case MeshType::Plane:
                            desc = MeshDesc::plane(planeW, planeH, subX, subZ);
                            break;
                        case MeshType::Cube:
                        default:
                            desc = MeshDesc::cube(meshSize);
                            break;
                    }

                    scene_.attachMesh(e, desc);
                    if (world.hasComponent<MeshRendererComponent>(e)) {
                        auto &mr           = world.getComponent<MeshRendererComponent>(e);
                        mr.visible         = visible;
                        mr.material.tint   = {tr, tg, tb, ta};
                        mr.material.tiling = tiling;
                    }
                }
            }

            if (mask & COMP_LIGHT) {
                LightComponent l;
                uint8_t        typeVal = 0;
                read(f, typeVal);
                l.type = static_cast<LightType>(typeVal);
                read(f, l.color.x);
                read(f, l.color.y);
                read(f, l.color.z);
                read(f, l.intensity);
                read(f, l.range);
                read(f, l.attenLinear);
                read(f, l.attenQuad);
                read(f, l.direction.x);
                read(f, l.direction.y);
                read(f, l.direction.z);
                read(f, l.innerAngle);
                read(f, l.outerAngle);
                read(f, l.castShadows);
                world.addComponent<LightComponent>(e, l);
            }
            if (mask & COMP_TEXTURE) {
                TextureComponent tc;
                tc.path           = readStr(f);
                uint8_t filterVal = 0;
                read(f, filterVal);
                tc.filter = static_cast<FilterMode>(filterVal);
                world.addComponent<TextureComponent>(e, tc);
                scene_.applyTexture(e);
            }

            const size_t count = std::min(static_cast<size_t>(savedExtCount), extensions_.size());
            for (size_t j = 0; j < count; ++j)
                if (extMask & (1u << j))
                    extensions_[j].load(f, e, world);
        }

        POSITRON_INFO("SceneSerializer: loaded {} entities from '{}'", entityCount, path);
        return true;
    }
} // namespace Positron
