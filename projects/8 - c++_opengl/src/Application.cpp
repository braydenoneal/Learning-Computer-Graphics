#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "renderer/Renderer.h"
#include "buffers/VertexBuffer.h"
#include "buffers/VertexBufferLayout.h"
#include "buffers/IndexBuffer.h"
#include "buffers/VertexArray.h"
#include "shaders/Shader.h"
#include "textures/Texture.h"

int width = 1920;
int height = 1080;

float ratio = (float) width / (float) height;

glm::mat4 pers = glm::perspective(glm::radians(70.0f), ratio, 0.1f, 100.0f);

bool cursor_disabled = false;
glm::vec4 camera = glm::vec4(0.0f, 0.0f, -5.5f, 1.0f);
bool w_pressed = false;
bool s_pressed = false;
bool a_pressed = false;
bool d_pressed = false;
bool space_pressed = false;
bool shift_pressed = false;

double camera_x = 0.0f;
double camera_y = 0.0f;

double pause_x = (float) width / 2.0f;
double pause_y = (float) height / 2.0f;

bool vsync_enabled = true;

int main2() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    int count;
    GLFWmonitor **monitors = glfwGetMonitors(&count);
    const GLFWvidmode *videoMode = glfwGetVideoMode(monitors[0]);
    int monitor_width = videoMode->width;
    int monitor_height = videoMode->height;

    width = monitor_width;
    height = monitor_height;
    ratio = (float) width / (float) height;

//    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    GLFWwindow *window = glfwCreateWindow(monitor_width, monitor_height, "Test", monitors[0], nullptr);

    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    {
        unsigned int tris_count = 12;

        float positions[] = {
                -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.6f,
                -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.6f,
                 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.6f,
                 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.6f,
                 0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.6f,

                 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.6f,
                 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  0.6f,
                -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.6f,
                -0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.6f,
                -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.6f,

                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.8f,
                -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  0.8f,
                -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.8f,
                -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.8f,
                -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.8f,

                 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.8f,
                 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.8f,
                 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.8f,
                 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.8f,
                 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.8f,

                -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,
                -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,
                 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  1.0f,  1.0f,
                 0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,

                 0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  2.0f,  0.4f,
                 0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  2.0f,  0.4f,
                -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  2.0f,  0.4f,
                -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  2.0f,  0.4f,
                -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  2.0f,  0.4f,
        };

        unsigned int indices[] = {
                0, 1, 2, 3, 4, 1,
                5, 6, 7, 8, 9, 6,
                10, 11, 12, 13, 14, 11,
                15, 16, 17, 18, 19, 16,
                20, 21, 22, 23, 24, 21,
                25, 26, 27, 28, 29, 26
        };

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        VertexArray va;
        VertexBuffer vb(positions, 30 * 7 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push(GL_FLOAT, 3, GL_FALSE);
        layout.Push(GL_FLOAT, 3, GL_FALSE);
        layout.Push(GL_FLOAT, 1, GL_FALSE);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, tris_count * 3);

        glm::vec3 angle = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 model_angle = glm::vec3(0.0f, 0.0f, 0.0f);

        Shader shader("../res/shaders/Basic.glsl");
        shader.Bind();

        std::vector<std::string> texture_paths = {
                "../res/textures/grass_block_side.png",
                "../res/textures/grass_block_top.png",
                "../res/textures/dirt.png"
        };

        Texture textures(texture_paths);
        textures.Bind(0);
        shader.SetUniform1i("u_Textures", 0);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();

        Renderer renderer;

        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void) io;
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330 core");
        ImGui::StyleColorsDark();

        bool spin_model = true;
        float spin_speed = 0.01f;

        glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
            if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
                cursor_disabled = !cursor_disabled;

                if (cursor_disabled) {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
                    glfwSetCursorPos(window, camera_x, camera_y);
                } else {
                    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                    glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
                    glfwSetCursorPos(window, pause_x, pause_y);
                }
            } else if (key == GLFW_KEY_W && action == GLFW_PRESS) {
                w_pressed = true;
            } else if (key == GLFW_KEY_W && action == GLFW_RELEASE) {
                w_pressed = false;
            } else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
                s_pressed = true;
            } else if (key == GLFW_KEY_S && action == GLFW_RELEASE) {
                s_pressed = false;
            } else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
                a_pressed = true;
            } else if (key == GLFW_KEY_A && action == GLFW_RELEASE) {
                a_pressed = false;
            } else if (key == GLFW_KEY_D && action == GLFW_PRESS) {
                d_pressed = true;
            } else if (key == GLFW_KEY_D && action == GLFW_RELEASE) {
                d_pressed = false;
            } else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
                space_pressed = true;
            } else if (key == GLFW_KEY_SPACE && action == GLFW_RELEASE) {
                space_pressed = false;
            } else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS) {
                shift_pressed = true;
            } else if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE) {
                shift_pressed = false;
            }
        });

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int e_width, int e_height) {
            glViewport(0, 0, e_width, e_height);
            width = e_width;
            height = e_height;

            ratio = (float) width / (float) height;
            pers = glm::perspective(glm::radians(70.0f), ratio, 0.1f, 100.0f);
        });

//        glfwSetWindowPos(window, monitor_width / 2 - width / 2, monitor_height / 2 - height / 2);
//        glfwDefaultWindowHints();
//        glfwShowWindow(window);

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            renderer.clear();

            glfwSwapInterval(vsync_enabled);

            if (cursor_disabled) {
                glfwGetCursorPos(window, &camera_x, &camera_y);
                angle.y = ((float) camera_x / (float) height) * 3.14f;
                angle.x = ((float) camera_y / (float) height) * 3.14f;

                if (w_pressed) {
                    glm::vec4 amount = glm::vec4(0.0f, 0.0f, 0.1f, 1.0f);
                    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::vec4 rotated_amount = rotate_mat * amount;
                    glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(rotated_amount));
                    camera = trans_mat * camera;
                }

                if (s_pressed) {
                    glm::vec4 amount = glm::vec4(0.0f, 0.0f, -0.1f, 1.0f);
                    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::vec4 rotated_amount = rotate_mat * amount;
                    glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(rotated_amount));
                    camera = trans_mat * camera;
                }

                if (a_pressed) {
                    glm::vec4 amount = glm::vec4(-0.1f, 0.0f, 0.0f, 1.0f);
                    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::vec4 rotated_amount = rotate_mat * amount;
                    glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(rotated_amount));
                    camera = trans_mat * camera;
                }

                if (d_pressed) {
                    glm::vec4 amount = glm::vec4(0.1f, 0.0f, 0.0f, 1.0f);
                    glm::mat4 rotate_mat = glm::rotate(glm::mat4(1.0f), angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    glm::vec4 rotated_amount = rotate_mat * amount;
                    glm::mat4 trans_mat = glm::translate(glm::mat4(1.0f), glm::vec3(rotated_amount));
                    camera = trans_mat * camera;
                }

                if (space_pressed) {
                    camera.y += 0.1f;
                }

                if (shift_pressed) {
                    camera.y -= 0.1f;
                }
            }

            if (spin_model) {
                if (model_angle.x > 3.14f) {
                    model_angle -= 3.14f * 2;
                }

                model_angle += spin_speed;
            }

            auto translate = glm::translate(glm::mat4(1.0f), -glm::vec3(camera.x, camera.y, -camera.z));

            auto model = glm::mat4(1.0f);
            model = glm::rotate(model, angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, angle.z, glm::vec3(0.0f, 0.0f, 1.0f));

            auto m_model = glm::mat4(1.0f);
            m_model = glm::rotate(m_model, model_angle.x, glm::vec3(1.0f, 0.0f, 0.0f));
            m_model = glm::rotate(m_model, model_angle.y, glm::vec3(0.0f, 1.0f, 0.0f));
            m_model = glm::rotate(m_model, model_angle.z, glm::vec3(0.0f, 0.0f, 1.0f));

            shader.SetUniformMatrix4fv("u_MVP", pers * model * translate * m_model);

            renderer.Draw(va, ib, shader);

            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            {
                ImGui::Begin("Debug");
                ImGui::SliderFloat3("Camera Angle", &angle.x, -3.14f, 3.14f);
                ImGui::SliderFloat3("Camera Position", &camera.x, -10.0f, 10.0f);
                ImGui::SliderFloat3("Model Angle", &model_angle.x, -3.14f, 3.14f);
                ImGui::SliderFloat("Spin speed", &spin_speed, 0.001f, 0.1f);
                ImGui::Checkbox("Spin Model", &spin_model);
                if (ImGui::Button("Reset Model Rotation")) {
                    model_angle.x = 0.0f;
                    model_angle.y = 0.0f;
                    model_angle.z = 0.0f;
                }
                ImGui::Checkbox("Vsync", &vsync_enabled);
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
                ImGui::End();
            }

            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}
