//
// Copyright (c) 2026 Noah Belton
// SPDX-License-Identifier: MIT
// Created by noahbelton29 on 25/03/2026.
//

#pragma once

#include "positron/core/export.h"

#include <GLFW/glfw3.h>

namespace Positron {
    enum class Key {
        // Letters
        A = GLFW_KEY_A,
        B = GLFW_KEY_B,
        C = GLFW_KEY_C,
        D = GLFW_KEY_D,
        E = GLFW_KEY_E,
        F = GLFW_KEY_F,
        G = GLFW_KEY_G,
        H = GLFW_KEY_H,
        I = GLFW_KEY_I,
        J = GLFW_KEY_J,
        K = GLFW_KEY_K,
        L = GLFW_KEY_L,
        M = GLFW_KEY_M,
        N = GLFW_KEY_N,
        O = GLFW_KEY_O,
        P = GLFW_KEY_P,
        Q = GLFW_KEY_Q,
        R = GLFW_KEY_R,
        S = GLFW_KEY_S,
        T = GLFW_KEY_T,
        U = GLFW_KEY_U,
        V = GLFW_KEY_V,
        W = GLFW_KEY_W,
        X = GLFW_KEY_X,
        Y = GLFW_KEY_Y,
        Z = GLFW_KEY_Z,

        // Numbers
        Num0 = GLFW_KEY_0,
        Num1 = GLFW_KEY_1,
        Num2 = GLFW_KEY_2,
        Num3 = GLFW_KEY_3,
        Num4 = GLFW_KEY_4,
        Num5 = GLFW_KEY_5,
        Num6 = GLFW_KEY_6,
        Num7 = GLFW_KEY_7,
        Num8 = GLFW_KEY_8,
        Num9 = GLFW_KEY_9,

        // Control
        Space     = GLFW_KEY_SPACE,
        Escape    = GLFW_KEY_ESCAPE,
        Enter     = GLFW_KEY_ENTER,
        Tab       = GLFW_KEY_TAB,
        Backspace = GLFW_KEY_BACKSPACE,
        Shift     = GLFW_KEY_LEFT_SHIFT,
        Control   = GLFW_KEY_LEFT_CONTROL,
        Alt       = GLFW_KEY_LEFT_ALT,

        // Arrow keys
        Up    = GLFW_KEY_UP,
        Down  = GLFW_KEY_DOWN,
        Left  = GLFW_KEY_LEFT,
        Right = GLFW_KEY_RIGHT,
    };

    enum class MouseButton {
        Left   = GLFW_MOUSE_BUTTON_LEFT,
        Right  = GLFW_MOUSE_BUTTON_RIGHT,
        Middle = GLFW_MOUSE_BUTTON_MIDDLE,
    };

    class POSITRON_API Input {
    public:
        static void init(GLFWwindow *window);

        [[nodiscard]] static bool isKeyDown(Key key);
        [[nodiscard]] static bool isKeyPressed(Key key);
        [[nodiscard]] static bool isKeyReleased(Key key);

        [[nodiscard]] static bool isMouseButtonDown(MouseButton button);

        [[nodiscard]] static double getMouseX();
        [[nodiscard]] static double getMouseY();

        static void update();

    private:
        static GLFWwindow *window_;
        static bool        prevKeys_[GLFW_KEY_LAST + 1];
        static bool        currKeys_[GLFW_KEY_LAST + 1];
    };
} // namespace Positron
