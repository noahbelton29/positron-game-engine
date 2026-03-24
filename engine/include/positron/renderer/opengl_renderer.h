//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include "positron/renderer/renderer.h"

namespace Positron {
    class OpenGLRenderer : public Renderer {
    public:
        bool init() override;
        void shutdown() override;

        void begin() override;
        void end() override;

        [[nodiscard]] RenderAPI   getAPI() const override { return RenderAPI::OpenGL; }
        [[nodiscard]] std::string getDeviceName() const override;
    };
} // namespace Positron
