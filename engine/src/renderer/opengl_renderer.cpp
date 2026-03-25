//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/renderer/opengl_renderer.h"
#include "positron/core/log.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

namespace Positron {

    static constexpr auto FALLBACK_VERT_SRC = R"(
        #version 460 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec3 aColor;
        layout (location = 2) in vec3 aNormal;
        layout (location = 3) in vec2 aUV;
        out vec3 vColor;
        uniform mat4 uMVP;
        void main() {
            vColor = aColor;
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";

    static constexpr const char *FALLBACK_FRAG_SRC = R"(
        #version 460 core
        in vec3 vColor;
        out vec4 fragColor;
        void main() { fragColor = vec4(vColor, 1.0); }
    )";

    bool OpenGLRenderer::init() {
        if (!GLContext::init())
            return false;

        std::cout << "Positron Engine\n"
                  << "  " << GLContext::getRenderer() << " (" << GLContext::getVendor() << ")\n"
                  << "  OpenGL " << GLContext::getVersion() << " | GLSL " << GLContext::getGLSLVersion() << "\n";

        defaultShader_ = GLShader::fromSource(FALLBACK_VERT_SRC, FALLBACK_FRAG_SRC);

        glEnable(GL_DEPTH_TEST);
        return true;
    }

    void OpenGLRenderer::shutdown() { delete defaultShader_; }

    void OpenGLRenderer::begin() {
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void OpenGLRenderer::end() {}

    void OpenGLRenderer::drawMesh(const Mesh *mesh, const glm::mat4 &transform) {
        if (!mesh)
            return;

        GLShader *active = customShader_ ? customShader_ : defaultShader_;

        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        const float aspect = viewport[2] > 0 && viewport[3] > 0
                                     ? static_cast<float>(viewport[2]) / static_cast<float>(viewport[3])
                                     : 1.0f;

        const glm::mat4     proj = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);
        constexpr glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -3.0f));
        const glm::mat4     mvp  = proj * view * transform;

        active->bind();
        active->setMat4("uMVP", glm::value_ptr(mvp));

        mesh->bind();
        glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
        mesh->unbind();

        active->unbind();
    }

    std::string OpenGLRenderer::getDeviceName() const { return GLContext::getRenderer(); }

} // namespace Positron
