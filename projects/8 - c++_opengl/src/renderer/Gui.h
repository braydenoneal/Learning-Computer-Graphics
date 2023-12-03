#pragma once

#include "imgui.h"
#include "GLFW/glfw3.h"

class Gui {
public:
    explicit Gui(GLFWwindow *window);

    void render();

    void terminate();

    ImGuiIO &getIo();
};
