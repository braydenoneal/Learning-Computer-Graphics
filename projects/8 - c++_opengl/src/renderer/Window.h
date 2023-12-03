#pragma once

#include "GLFW/glfw3.h"
#include <functional>

class Window {
private:
    GLFWmonitor **monitors;
    int monitorCount;
    const GLFWvidmode *videoMode;
    int windowWidth;
    int windowHeight;
    GLFWwindow *window;
public:
    Window();

    GLFWwindow *getWindow();

    int window_should_close();

    int getMonitorWidth();

    int getMonitorHeight();

    float get_window_aspect_ratio();

    void set_callback(GLFWkeyfun callback);

    void poll_events();

    void swap_buffers();

    void terminate();
};
