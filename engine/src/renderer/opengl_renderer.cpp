//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/renderer/opengl_renderer.h"

#include <glad/glad.h>
#include <iostream>

namespace Positron {
    bool OpenGLRenderer::init() {
        if (!gladLoadGL()) {
            std::cerr << "Failed to initialize GLAD\n";
            return false;
        }

        std::cout << "Positron Engine\n"
                  << "  " << glGetString(GL_RENDERER) << " (" << glGetString(GL_VENDOR) << ")\n"
                  << "  OpenGL " << glGetString(GL_VERSION) << " | GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << "\n";

        return true;
    }

    void OpenGLRenderer::shutdown() {}

    void OpenGLRenderer::begin() {
        glClearColor(0.5f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::end() {}

    std::string OpenGLRenderer::getDeviceName() const {
        return reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    }
} // namespace Positron
