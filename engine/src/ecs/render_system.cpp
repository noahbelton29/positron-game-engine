//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/ecs/systems/render_system.h"
#include "positron/ecs/component/components.h"
#include "positron/ecs/core/world.h"
#include "positron/ecs/scene/camera_data.h"
#include "positron/renderer/renderer.h"

#include <glm/glm.hpp>

namespace Positron {
    void RenderSystem::onUpdate(World &world) {
        if (!world.hasResource<Renderer *>())
            return;
        auto renderer = world.getResource<Renderer *>();

        CameraData camera;
        if (world.hasResource<CameraData>())
            camera = world.getResource<CameraData>();

        world.each<TransformComponent, MeshRendererComponent>(
                [renderer, &camera](Entity, const TransformComponent &transform,
                                    const MeshRendererComponent &meshRenderer) {
                    if (meshRenderer.visible && meshRenderer.mesh)
                        renderer->drawMesh(meshRenderer.mesh, transform.matrix(), meshRenderer.material, camera);
                });
    }
} // namespace Positron
