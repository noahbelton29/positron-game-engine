//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/renderer/opengl_renderer.h"
#include "positron/core/log.h"

#include <glad/glad.h>
#include <iostream>

namespace Positron {
    // fallback, always works even if asset files are missing for the default shaders
    static auto FALLBACK_VERT = R"(
        #version 460 core
        layout (location = 0) in vec3 aPos;
        void main() { gl_Position = vec4(aPos, 1.0); }
    )";

    static auto FALLBACK_FRAG = R"(
        #version 460 core
        out vec4 fragColor;
        void main() { fragColor = vec4(1.0, 0.5, 0.2, 1.0); }
    )";

    static constexpr float triangleVertices[] = {
            -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f,
    };

    bool OpenGLRenderer::init() {
        if (!gladLoadGL()) {
            std::cerr << "Failed to initialize GLAD\n";
            return false;
        }

        std::cout << "Positron Engine\n"
                  << "  " << glGetString(GL_RENDERER) << " (" << glGetString(GL_VENDOR) << ")\n"
                  << "  OpenGL " << glGetString(GL_VERSION) << " | GLSL " << glGetString(GL_SHADING_LANGUAGE_VERSION)
                  << "\n";

        // try loading from file, fall back to default source
        auto *loaded =
                new Shader("engine/assets/shaders/opengl/default.vert", "engine/assets/shaders/opengl/default.frag");

        if (loaded->getID() == 0) {
            POSITRON_WARN("Default shader files not found, using default fallback");
            delete loaded;
            defaultShader_ = Shader::fromSource(FALLBACK_VERT, FALLBACK_FRAG);
        } else {
            defaultShader_ = loaded;
        }

        // VAO/VBO
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);

        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangleVertices), triangleVertices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0);

        return true;
    }

    void OpenGLRenderer::shutdown() {
        delete defaultShader_;
        glDeleteVertexArrays(1, &vao_);
        glDeleteBuffers(1, &vbo_);
    }

    void OpenGLRenderer::begin() {
        glClearColor(0.5f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const Shader *active = customShader_ ? customShader_ : defaultShader_;

        active->bind();
        glBindVertexArray(vao_);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
        active->unbind();
    }

    void OpenGLRenderer::end() {}

    std::string OpenGLRenderer::getDeviceName() const {
        return reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    }
} // namespace Positron
