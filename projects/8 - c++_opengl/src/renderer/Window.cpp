#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Window.h"

Window::Window() : windowWidth(1920), windowHeight(1080) {
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    monitors = glfwGetMonitors(&monitorCount);
    videoMode = glfwGetVideoMode(monitors[0]);

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(windowWidth, windowHeight, "Test", nullptr, nullptr);

    if (window == nullptr) {
        glfwTerminate();
    }

    glfwMakeContextCurrent(window);

    glfwSetWindowPos(window, getMonitorWidth() / 2 - windowWidth / 2, getMonitorHeight() / 2 - windowHeight / 2);
    glfwDefaultWindowHints();
    glfwShowWindow(window);

    glfwSwapInterval(1);

    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
}

int Window::window_should_close() {
    return glfwWindowShouldClose(window);
}

int Window::getMonitorWidth() {
    return videoMode->width;
}

int Window::getMonitorHeight() {
    return videoMode->height;
}

float Window::get_window_aspect_ratio() {
    return (float) windowWidth / (float) windowHeight;
}

void Window::poll_events() {
    glfwPollEvents();
}

void Window::swap_buffers() {
    glfwSwapBuffers(window);
}

void Window::terminate() {
    glfwTerminate();
}

GLFWwindow *Window::getWindow() {
    return window;
}

void Window::set_callback(GLFWkeyfun callback) {
    glfwSetKeyCallback(window, callback);
}
