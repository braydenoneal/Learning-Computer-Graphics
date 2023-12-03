#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "../window/window.hpp"

namespace input {
    void key_callback(GLFWwindow* glfw_window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            switch (key) {
                case GLFW_KEY_F11:
                    window::toggle_maximize();
                    break;
                case GLFW_KEY_Q:
                    window::close();
                    break;
                default:
                    break;
            }
        }
    }

    void create_context() {
        glfwSetKeyCallback(window::get_glfw_window(), key_callback);
    }

    void poll_events() {
        glfwPollEvents();
    }
}
