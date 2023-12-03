#include <map>
#include <list>
#include <iostream>

class Input {
public:
    WindowContext *window_context;
    GameContext *game_context;

    std::map<int, int> keys;

    Input(WindowContext *input_window_context, GameContext *input_game_context) {
        window_context = input_window_context;
        game_context = input_game_context;

        game_context->default_cursor_x = (double) window_context->window_width / 2;
        game_context->default_cursor_y = (double) window_context->window_height / 2;

        glfwSetWindowUserPointer(window_context->window, this);
        glfwSetKeyCallback(window_context->window, key_callback);
        glfwSetFramebufferSizeCallback(window_context->window, resize_callback);
    }

    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        auto *input = (Input *) glfwGetWindowUserPointer(window);

        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
            if (input->game_context->paused) {
                glfwSetInputMode(input->window_context->window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                glfwSetInputMode(input->window_context->window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                glfwSetCursorPos(input->window_context->window, input->game_context->camera_cursor_x, input->game_context->camera_cursor_y);
                input->game_context->paused = false;
            } else {
                glfwSetInputMode(input->window_context->window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                glfwSetInputMode(input->window_context->window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                glfwSetCursorPos(input->window_context->window, input->game_context->default_cursor_x, input->game_context->default_cursor_y);
                input->game_context->paused = true;
            }
        }

        if (key == GLFW_KEY_Q && action == GLFW_PRESS) {
            WindowContext::terminate();
            exit(0);
        }

        input->keys[key] = action;
    }

    static void resize_callback(GLFWwindow* window, int width, int height) {
        if (width > 0 && height > 0) {
            glViewport(0, 0, width, height);
            auto *input = (Input *) glfwGetWindowUserPointer(window);
            input->window_context->window_width = width;
            input->window_context->window_height = height;
            input->game_context->default_cursor_x = (double) width / 2;
            input->game_context->default_cursor_y = (double) height / 2;
        }
    }

    void do_actions() {
        set_mouse_position();

        for (std::pair<const int, int> key : keys) {
            if (keys[key.first]) {
                Action::do_action(key.first, window_context, game_context);
            }
        }

        if (!game_context->paused) {
            game_context->camera_angle.y = M_PI * ((float) game_context->camera_cursor_x / (float) window_context->window_height);
            game_context->camera_angle.x = M_PI * ((float) game_context->camera_cursor_y / (float) window_context->window_height);
        }
    }

    void set_mouse_position() const {
        if (!game_context->paused) {
            glfwGetCursorPos(window_context->window, &game_context->camera_cursor_x, &game_context->camera_cursor_y);
        }
    }
};
