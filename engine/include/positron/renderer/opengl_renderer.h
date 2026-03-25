//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include "positron/core/export.h"
#include "positron/renderer/renderer.h"
#include "positron/renderer/shader.h"

namespace Positron {
    class POSITRON_API OpenGLRenderer : public Renderer {
    public:
        bool init() override;
        void shutdown() override;
        void begin() override;
        void end() override;

        [[nodiscard]] RenderAPI   getAPI() const override { return RenderAPI::OpenGL; }
        [[nodiscard]] std::string getDeviceName() const override;

    private:
        Shader      *defaultShader_ = nullptr;
        unsigned int vao_           = 0;
        unsigned int vbo_           = 0;
    };
} // namespace Positron
