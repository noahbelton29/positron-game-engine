//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include <string>

namespace Positron {
    enum class RenderAPI { None = 0, OpenGL = 1, Vulkan = 2 };

    class Shader;

    class Renderer {
    public:
        virtual ~Renderer() = default;

        virtual bool init()     = 0;
        virtual void shutdown() = 0;
        virtual void begin()    = 0;
        virtual void end()      = 0;

        void setShader(Shader *shader) { customShader_ = shader; }

        [[nodiscard]] virtual RenderAPI   getAPI() const        = 0;
        [[nodiscard]] virtual std::string getDeviceName() const = 0;

        [[nodiscard]] static Renderer *create(RenderAPI api);

    protected:
        Shader *customShader_ = nullptr;
    };
} // namespace Positron
