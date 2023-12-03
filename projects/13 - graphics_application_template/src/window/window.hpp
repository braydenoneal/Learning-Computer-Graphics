#pragma once

#include <string>

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "graphics/shader.hpp"

static bool maximized = false;
static std::string title = "Graphics Application Template";
static int width = 640;
static int height = 480;

static GLFWwindow *glfw_window;

namespace window {
    void create_context() {
        glfwInit();

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, maximized ? GL_TRUE : GLFW_FALSE);

        glfw_window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        glfwMakeContextCurrent(glfw_window);

        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        glfwSwapInterval(1);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(glfw_window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
        ImGui::StyleColorsDark();
    }

    int should_close() {
        return glfwWindowShouldClose(glfw_window);
    }

    void terminate() {
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
        glfwTerminate();
    }

    void close() {
        glfwSetWindowShouldClose(glfw_window, GLFW_TRUE);
    }

    GLFWwindow *get_glfw_window() {
        return glfw_window;
    }

    void toggle_maximize() {
        maximized = !maximized;

        if (maximized) {
            glfwMaximizeWindow(glfw_window);
        } else {
            glfwRestoreWindow(glfw_window);
        }
    }

    void update() {}

    void render() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        GLuint vertex_array;
        glGenVertexArrays(1, &vertex_array);
        glBindVertexArray(vertex_array);

        GLuint vertex_buffer;
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        float vertex_buffer_data[9] = {0, 0, 0, 0, 1, 0, 1, 1, 0};
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0); // index
        glVertexAttribPointer(
                0,          // GLuint	    index,
                3,          // GLint	    size,
                GL_FLOAT,   // GLenum	    type,
                GL_FALSE,   // GLboolean	normalized,
                4 * 3,      // GLsizei	    stride,
                (GLvoid *) 0
        );

        GLuint program = shader::create_shaders(
                "./window/graphics/shaders/default/vertex.glsl",
                "./window/graphics/shaders/default/fragment.glsl"
        );

        shader::bind(program);

        glDrawArrays(GL_TRIANGLES, 0, 9 * 4);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Debug");
            if (ImGui::Button("Maximize")) {
                toggle_maximize();
            }
            ImGui::Text("%.3f ms/frame %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(glfw_window);
    }
}
