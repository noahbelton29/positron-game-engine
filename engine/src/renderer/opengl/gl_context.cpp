//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#include "positron/renderer/opengl/gl_context.h"

#include <glad/glad.h>
#include <iostream>

namespace Positron {
    bool GLContext::init() {
        if (!gladLoadGL()) {
            std::cerr << "Failed to initialize GLAD\n";
            return false;
        }
        return true;
    }

    void GLContext::shutdown() {}

    std::string GLContext::getRenderer() { return reinterpret_cast<const char *>(glGetString(GL_RENDERER)); }

    std::string GLContext::getVendor() { return reinterpret_cast<const char *>(glGetString(GL_VENDOR)); }

    std::string GLContext::getVersion() { return reinterpret_cast<const char *>(glGetString(GL_VERSION)); }

    std::string GLContext::getGLSLVersion() {
        return reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    }
} // namespace Positron
