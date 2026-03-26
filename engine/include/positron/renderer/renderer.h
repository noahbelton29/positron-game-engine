//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <utility>
#include "positron/ecs/component/components.h"
#include "positron/renderer/mesh.h"

#include "positron/ecs/scene/camera_data.h"
#include "positron/ecs/scene/light_data.h"

namespace Positron {
    enum class RenderAPI { None = 0, OpenGL = 1, Vulkan = 2 };

    class GLShader;

    class Renderer {
    public:
        virtual ~Renderer() = default;

        virtual bool init()     = 0;
        virtual void shutdown() = 0;
        virtual void begin()    = 0;
        virtual void end()      = 0;

        virtual void shadowPass(const LightData& lights,
                                const std::vector<std::pair<glm::mat4, const Mesh*>>& drawList) = 0;

        virtual void drawMesh(const Mesh* mesh, const glm::mat4& transform,
                              const MaterialComponent& material,
                              const CameraData& camera,
                              const LightData& lights) = 0;

        void setShader(GLShader* shader) { customShader_ = shader; }

        [[nodiscard]] virtual RenderAPI   getAPI() const        = 0;
        [[nodiscard]] virtual std::string getDeviceName() const = 0;

        [[nodiscard]] static Renderer* create(RenderAPI api);

    protected:
        GLShader* customShader_ = nullptr;
    };
}
