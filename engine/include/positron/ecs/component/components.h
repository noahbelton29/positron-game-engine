//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/ecs/scene/mesh_type.h"
#include "positron/renderer/geometry.h"
#include "positron/renderer/mesh.h"
#include "positron/renderer/texture.h"

#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>

#include "positron/ecs/core/entity.h"

namespace Positron {
    struct POSITRON_API TransformComponent {
        glm::vec3 position        = {0.f, 0.f, 0.f};
        glm::vec3 rotation        = {0.f, 0.f, 0.f};
        glm::vec3 scale           = {1.f, 1.f, 1.f};
        glm::quat physicsRotation = glm::quat(1.f, 0.f, 0.f, 0.f);

        [[nodiscard]] glm::mat4 matrix() const;
    };

    struct POSITRON_API MaterialComponent {
        Texture  *albedo = nullptr;
        glm::vec4 tint   = {1.f, 1.f, 1.f, 1.f};
        float     tiling = 1.f;
    };

    struct POSITRON_API TextureComponent {
        std::string path;
        FilterMode  filter = FilterMode::Linear;
    };

    struct POSITRON_API MeshRendererComponent {
        Mesh             *mesh     = nullptr;
        MaterialComponent material = {};
        bool              visible  = true;
    };

    struct POSITRON_API TagComponent {
        std::string name;
    };

    struct POSITRON_API MeshDescComponent {
        MeshType type   = MeshType::Cube;
        float    size   = 1.f;
        float    planeW = 1.f;
        float    planeH = 1.f;
        int      subX   = 1;
        int      subZ   = 1;
    };

    struct POSITRON_API RigidbodyComponent {
        glm::vec3 linearVelocity  = {0.f, 0.f, 0.f};
        glm::vec3 angularVelocity = {0.f, 0.f, 0.f};
        float     mass            = 1.f;
        float     linearDamping   = 0.05f;
        float     angularDamping  = 0.05f;
        float     restitution     = 0.3f;
        float     friction        = 0.6f;
        bool      isKinematic     = false;
        bool      useGravity      = true;

        uint32_t _bodyId = UINT32_MAX;
    };

    struct POSITRON_API MeshColliderComponent {
        GeometryData geometry;
        uint32_t     _bodyId = UINT32_MAX;
    };

    struct POSITRON_API BoxColliderComponent {
        glm::vec3 halfExtents = {0.5f, 0.5f, 0.5f};
        glm::vec3 offset      = {0.f, 0.f, 0.f};
        uint32_t  _bodyId     = UINT32_MAX;
    };

    struct POSITRON_API CollisionInfo {
        Entity    other;
        glm::vec3 contactPoint;
        glm::vec3 contactNormal;
    };

    struct POSITRON_API CollisionCallbackComponent {
        std::function<void(const CollisionInfo &)> onContactAdded;
        std::function<void(const CollisionInfo &)> onContactPersisted;
        std::function<void(const CollisionInfo &)> onContactRemoved;
    };

    enum class LightType { Directional = 0, Point = 1, Spot = 2 };

    struct POSITRON_API LightComponent {
        LightType type      = LightType::Point;
        glm::vec3 color     = {1.f, 1.f, 1.f};
        float     intensity = 1.f;

        float range       = 10.f;
        float attenLinear = 0.09f;
        float attenQuad   = 0.032f;

        glm::vec3 direction  = {0.f, -1.f, 0.f};
        float     innerAngle = 12.5f;
        float     outerAngle = 17.5f;

        bool castShadows = false;
    };
} // namespace Positron
