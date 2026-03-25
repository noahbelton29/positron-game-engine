//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
//

#pragma once

#include "positron/core/export.h"
#include "positron/renderer/opengl/gl_context.h"
#include "positron/renderer/opengl/gl_shader.h"
#include "positron/renderer/renderer.h"

namespace Positron {
    class POSITRON_API OpenGLRenderer : public Renderer {
    public:
        bool init() override;
        void shutdown() override;
        void begin() override;
        void end() override;

        void drawMesh(const Mesh *mesh, const glm::mat4 &transform) override;

        [[nodiscard]] RenderAPI   getAPI() const override { return RenderAPI::OpenGL; }
        [[nodiscard]] std::string getDeviceName() const override;

    private:
        GLShader *defaultShader_ = nullptr;
    };
} // namespace Positron
