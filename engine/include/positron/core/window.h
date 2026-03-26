//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <GLFW/glfw3.h>
#include <string>

namespace Positron {
    struct POSITRON_API WindowConfig {
        std::string  title  = "Positron Engine"; // NOLINT
        unsigned int width  = 1280;
        unsigned int height = 720;
    };

    class POSITRON_API Window {
    public:
        explicit Window(WindowConfig config = WindowConfig());
        ~Window();

        bool init();
        void onUpdate() const;
        void shutdown();

        [[nodiscard]] unsigned int getWidth() const;
        [[nodiscard]] unsigned int getHeight() const;
        [[nodiscard]] bool         shouldClose() const;
        [[nodiscard]] GLFWwindow  *getHandle() const { return window_; }

        void setMouseGrabbed(bool grabbed) const;

    private:
        GLFWwindow  *window_ = nullptr;
        WindowConfig config_;

        void registerCallbacks();
    };
} // namespace Positron
