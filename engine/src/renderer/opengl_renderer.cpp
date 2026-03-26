//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 26/03/2026.
//

#include "positron/renderer/opengl_renderer.h"
#include "positron/core/log.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
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
        out vec2 vUV;

        uniform mat4 uMVP;
        uniform float uTiling;

        void main() {
            vColor      = aColor;
            vUV         = aUV * uTiling;
            gl_Position = uMVP * vec4(aPos, 1.0);
        }
    )";

    static constexpr auto FALLBACK_FRAG_SRC = R"(
        #version 460 core

        in vec3 vColor;
        in vec2 vUV;

        out vec4 fragColor;

        uniform sampler2D uAlbedo;
        uniform int       uHasTexture;
        uniform vec4      uTint;

        void main() {
            vec4 base = uHasTexture == 1
                ? texture(uAlbedo, vUV)
                : vec4(vColor, 1.0);

            fragColor = base * uTint;
        }
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

    void OpenGLRenderer::drawMesh(const Mesh *mesh, const glm::mat4 &transform, const MaterialComponent &material,
                                  const CameraData &camera) {
        if (!mesh)
            return;

        GLShader *activeShader = customShader_ ? customShader_ : defaultShader_;

        const glm::mat4 mvp = camera.projection * camera.view * transform;

        activeShader->bind();
        activeShader->setMat4("uMVP", glm::value_ptr(mvp));
        activeShader->setVec4("uTint", material.tint.r, material.tint.g, material.tint.b, material.tint.a);
        activeShader->setFloat("uTiling", material.tiling);

        if (material.albedo) {
            material.albedo->bind(0);
            activeShader->setInt("uAlbedo", 0);
            activeShader->setInt("uHasTexture", 1);
        } else {
            activeShader->setInt("uHasTexture", 0);
        }

        mesh->bind();
        glDrawElements(GL_TRIANGLES, mesh->getIndexCount(), GL_UNSIGNED_INT, nullptr);
        mesh->unbind();

        if (material.albedo)
            material.albedo->unbind();

        activeShader->unbind();
    }

    std::string OpenGLRenderer::getDeviceName() const { return GLContext::getRenderer(); }
} // namespace Positron
