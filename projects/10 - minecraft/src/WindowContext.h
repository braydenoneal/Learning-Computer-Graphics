class WindowContext {
public:
    GLFWwindow *window;
    int window_width{1920};
    int window_height{1080};
    float field_of_view{70};

    WindowContext() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

        window = glfwCreateWindow(window_width, window_height, "Minecraft", nullptr, nullptr);

        glfwMakeContextCurrent(window);

        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        glfwSwapInterval(1);

        glfwGetWindowSize(window, &window_width, &window_height);
    }

    [[nodiscard]] int window_should_close() const {
        return glfwWindowShouldClose(window);
    }

    [[nodiscard]] float get_aspect_ratio() const {
        return (float) window_width / (float) window_height;
    }

    void swap_buffers() const {
        glfwSwapBuffers(window);
    }

    static void handle_events() {
        glfwPollEvents();
    }

    static void terminate() {
        glfwTerminate();
    }
};
