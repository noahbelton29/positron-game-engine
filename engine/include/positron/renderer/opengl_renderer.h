//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
//

#pragma once

#include "positron/core/export.h"
#include "positron/renderer/opengl/gl_context.h"
#include "positron/renderer/opengl/gl_shader.h"
#include "positron/renderer/renderer.h"

#include <array>

namespace Positron {
    inline constexpr int MAX_SHADOW_MAPS = 4;
    inline constexpr int SHADOW_MAP_SIZE = 4096;

    class POSITRON_API OpenGLRenderer : public Renderer {
    public:
        bool init() override;
        void shutdown() override;
        void begin() override;
        void end() override;

        void drawMesh(const Mesh *mesh, const glm::mat4 &transform, const MaterialComponent &material,
                      const CameraData &camera, const LightData &lights) override;

        void shadowPass(const LightData                                       &lights,
                        const std::vector<std::pair<glm::mat4, const Mesh *>> &drawList) override;

        [[nodiscard]] RenderAPI   getAPI() const override { return RenderAPI::OpenGL; }
        [[nodiscard]] std::string getDeviceName() const override;

    private:
        void initShadowMaps();
        void shutdownShadowMaps() const;

        GLShader *defaultShader_ = nullptr;
        GLShader *depthShader_   = nullptr;

        std::array<unsigned int, MAX_SHADOW_MAPS> shadowFBOs_ = {};
        std::array<unsigned int, MAX_SHADOW_MAPS> shadowTexs_ = {};
    };
} // namespace Positron
