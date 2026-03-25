//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 24/03/2026.
//

#include "positron/core/window.h"
#include "positron/core/event.h"
#include "positron/core/event_bus.h"

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
        registerCallbacks();
        return true;
    }

    void Window::registerCallbacks() {
        glfwSetWindowUserPointer(window_, this);

        glfwSetWindowCloseCallback(window_, [](GLFWwindow *) {
            WindowCloseEvent e;
            EventBus::emit(e);
        });

        glfwSetWindowSizeCallback(window_, [](GLFWwindow *, int w, int h) {
            WindowResizeEvent e(static_cast<unsigned>(w), static_cast<unsigned>(h));
            EventBus::emit(e);
        });

        glfwSetKeyCallback(window_, [](GLFWwindow *, int glfwKey, int /*scancode*/, int action, int /*mods*/) {
            if (glfwKey < 0)
                return;
            const auto key = static_cast<Key>(glfwKey);
            if (action == GLFW_PRESS) {
                KeyPressedEvent e(key);
                EventBus::emit(e);
            } else if (action == GLFW_RELEASE) {
                KeyReleasedEvent e(key);
                EventBus::emit(e);
            }
        });

        glfwSetMouseButtonCallback(window_, [](GLFWwindow *, int button, int action, int /*mods*/) {
            const auto btn = static_cast<MouseButton>(button);
            if (action == GLFW_PRESS) {
                MouseButtonPressedEvent e(btn);
                EventBus::emit(e);
            } else if (action == GLFW_RELEASE) {
                MouseButtonReleasedEvent e(btn);
                EventBus::emit(e);
            }
        });

        glfwSetCursorPosCallback(window_, [](GLFWwindow *, double x, double y) {
            MouseMovedEvent e(x, y);
            EventBus::emit(e);
        });

        glfwSetScrollCallback(window_, [](GLFWwindow *, double xOff, double yOff) {
            MouseScrolledEvent e(xOff, yOff);
            EventBus::emit(e);
        });
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
