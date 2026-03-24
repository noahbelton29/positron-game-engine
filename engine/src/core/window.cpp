//
// Copyright (c) 2025 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/core/window.h"

#include <iostream>
#include <utility>

namespace Positron {
    Window::Window(WindowConfig config) : config_(std::move(config)) {}

    Window::~Window() { shutdown(); }

    bool Window::init() {
        if (!glfwInit()) {
            std::cerr << "Failed to initialize GLFW\n";
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window_ = glfwCreateWindow(static_cast<int>(config_.width), static_cast<int>(config_.height),
                                   config_.title.c_str(), nullptr, nullptr);

        if (!window_) {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return false;
        }

        glfwMakeContextCurrent(window_);
        return true;
    }

    void Window::onUpdate() const {
        glfwSwapBuffers(window_);
        glfwPollEvents();
    }

    void Window::shutdown() {
        if (window_) {
            glfwDestroyWindow(window_);
            window_ = nullptr;
        }
        glfwTerminate();
    }

    unsigned int Window::getWidth() const { return config_.width; }
    unsigned int Window::getHeight() const { return config_.height; }
    bool         Window::shouldClose() const { return glfwWindowShouldClose(window_); }

} // namespace Positron
