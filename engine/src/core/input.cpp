#include "positron/core/input.h"

#include <cstring>

namespace Positron {
    GLFWwindow *Input::window_                      = nullptr;
    bool        Input::prevKeys_[GLFW_KEY_LAST + 1] = {};
    bool        Input::currKeys_[GLFW_KEY_LAST + 1] = {};

    void Input::init(GLFWwindow *window) { window_ = window; }

    void Input::update() {
        memcpy(prevKeys_, currKeys_, sizeof(currKeys_));

        for (int i = 0; i <= GLFW_KEY_LAST; ++i)
            currKeys_[i] = glfwGetKey(window_, i) == GLFW_PRESS;
    }

    bool Input::isKeyDown(Key key) { return currKeys_[static_cast<int>(key)]; }

    bool Input::isKeyPressed(Key key) {
        const int k = static_cast<int>(key);
        return currKeys_[k] && !prevKeys_[k];
    }

    bool Input::isKeyReleased(Key key) {
        const int k = static_cast<int>(key);
        return !currKeys_[k] && prevKeys_[k];
    }

    bool Input::isMouseButtonDown(MouseButton button) {
        return glfwGetMouseButton(window_, static_cast<int>(button)) == GLFW_PRESS;
    }

    double Input::getMouseX() {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        return x;
    }

    double Input::getMouseY() {
        double x, y;
        glfwGetCursorPos(window_, &x, &y);
        return y;
    }
} // namespace Positron
