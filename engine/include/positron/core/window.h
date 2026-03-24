//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include <GLFW/glfw3.h>

#include <string>
#include <utility>

namespace Positron {
    struct WindowConfig {
        std::string  title  = "Positron Engine"; // NOLINT
        unsigned int width  = 1280;
        unsigned int height = 720;
    };

    class Window {
    public:
        explicit Window(WindowConfig config = WindowConfig());
        ~Window();

        bool init();
        void onUpdate() const;
        void shutdown();

        [[nodiscard]] unsigned int getWidth() const;
        [[nodiscard]] unsigned int getHeight() const;
        [[nodiscard]] bool         shouldClose() const;

    private:
        GLFWwindow  *window_ = nullptr;
        WindowConfig config_;
    };
} // namespace Positron
