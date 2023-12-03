#pragma once

#include <GLFW/glfw3.h>
#include <functional>

#include "Context.h"

class KeyAction {
private:
    int key;
    std::function<void(Context context)> action;
public:
    KeyAction(int input_key, std::function<void(Context context)> input_action) {
        key = input_key;
        action = input_action;
    };
};
