#pragma once

#include <GLFW/glfw3.h>
#include <map>
#include <list>

#include "KeyAction.h"

class Keymap {
private:
    std::map<int, bool> keys;
    std::list<KeyAction> key_actions;
public:
    Keymap() {
        key_actions = {
                KeyAction(GLFW_KEY_W, [](Context context) { context.camera_position += 0.1f; }),
        };
    }

    std::list<KeyAction> get_key_actions() const { return key_actions; }

    void callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
        keys[key] = action;
    };

//    void set_callback(Window window) {
//        window.set_callback([](GLFWwindow *window, int key, int scancode, int action, int mods) {
//            keys[key] = action;
//        });
//    }
};
