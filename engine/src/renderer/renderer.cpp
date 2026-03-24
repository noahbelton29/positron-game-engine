//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/renderer/renderer.h"
#include "positron/renderer/opengl_renderer.h"

#include <iostream>

namespace Positron {
    Renderer *Renderer::create(const RenderAPI api) {
        switch (api) {
            case RenderAPI::OpenGL:
                return new OpenGLRenderer();
            default:
                std::cerr << "Unsupported RenderAPI\n";
                return nullptr;
        }
    }
} // namespace Positron
