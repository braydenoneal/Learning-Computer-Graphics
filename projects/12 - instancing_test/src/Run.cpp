#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <cmath>
#include <cstdlib>
#include <map>
#include <glm/gtc/noise.hpp>
#include <glm/gtc/random.hpp>
#include <stb_image.h>

#include "render/VertexArray.h"
#include "render/VertexBuffer.h"
#include "render/Shader.h"
#include "render/Transform.h"
#include "world/Chunk.h"

const int world_size = 256;
auto light_position = glm::vec3(world_size / 2.0f, world_size * 0.85f, -world_size / 2.0f);
auto camera_position = glm::vec3(world_size / 2.0f, world_size * 0.85f, -world_size / 2.0f);
auto camera_angle = glm::vec3(M_PI / 2.0f, 0.0f, 0.0f);
float field_of_view = 70.0f;
float speed = 2.0f;
bool paused = true;
int window_width, window_height;
double mx, my;

std::map<int, int> keys;

typedef std::array<float, world_size * world_size> position_data_t;

struct data_t {
    glm::vec3 vertex;
    glm::vec3 normal;
};

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   keys[key] = action;

    if (key == GLFW_KEY_Q) {
        glfwTerminate();
        exit(0);
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        if (paused) {  // Unpause
            paused = false;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
            glfwSetCursorPos(window, camera_angle.y / M_PI * (float) window_height, camera_angle.x / M_PI * (float) window_height);
        } else {  // Pause
            paused = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_FALSE);
            glfwSetCursorPos(window, (float) window_width / 2, (float) window_height / 2);
        }
    }
}

float get_noise_value(int x, int z) {
    float scale = 150.0f;
    float faults = 1.5f * glm::perlin(glm::vec2((float) x / scale, (float) z / scale));
    float faults2 = 0.125f * glm::perlin(glm::vec2((float) x / scale * 16, (float) z / scale * 16));
    faults += faults2;
    float noise = 2.0f * glm::perlin(glm::vec2((float) x / scale, (float) z / scale));
    float noise2 = 0.5f * glm::perlin(glm::vec2((float) x / scale * 2, (float) z / scale * 2));
    float noise3 = 0.25f * glm::perlin(glm::vec2((float) x / scale * 4, (float) z / scale * 4));
    float noise4 = 0.125f * glm::perlin(glm::vec2((float) x / scale * 8, (float) z / scale * 8));
    float y = (noise + noise2 + noise3 + noise4) * 0.5f + 0.5f;
    faults *= 2;
    if (faults > 0 && faults < 0.4f) {
        y += faults;
    } else if (faults >= 0.4f) {
        y += 0.4f;
    }
    float height_scale = 64.0f;
    return height_scale * y;
}

void erode(position_data_t &position_data, int x, int z, float payload, int step) {
    if (x > 0 && z > 0 && x < world_size - 1 && z < world_size - 1) {
        float negative_x_negative_z = position_data[(x - 1) * world_size + (z - 1)];
        float negative_x_z = position_data[(x - 1) * world_size + (z)];
        float negative_x_positive_z = position_data[(x - 1) * world_size + (z + 1)];
        float x_negative_z = position_data[(x) * world_size + (z - 1)];
        float x_z = position_data[(x) * world_size + (z)];
        float x_positive_z = position_data[(x) * world_size + (z + 1)];
        float positive_x_negative_z = position_data[(x + 1) * world_size + (z - 1)];
        float positive_x_z = position_data[(x + 1) * world_size + (z)];
        float positive_x_positive_z = position_data[(x + 1) * world_size + (z + 1)];

        float min = x_z;
        int min_x = x;
        int min_z = z;

        if (negative_x_negative_z < min) {
            min = negative_x_negative_z;
            min_x = x - 1;
            min_z = z - 1;
        }
        if (negative_x_z < min) {
            min = negative_x_z;
            min_x = x - 1;
            min_z = z;
        }
        if (negative_x_positive_z < min) {
            min = negative_x_positive_z;
            min_x = x - 1;
            min_z = z + 1;
        }
        if (x_negative_z < min) {
            min = x_negative_z;
            min_x = x;
            min_z = z - 1;
        }
        if (x_positive_z < min) {
            min = x_positive_z;
            min_x = x;
            min_z = z + 1;
        }
        if (positive_x_negative_z < min) {
            min = positive_x_negative_z;
            min_x = x + 1;
            min_z = z - 1;
        }
        if (positive_x_z < min) {
            min = positive_x_z;
            min_x = x + 1;
            min_z = z;
        }
        if (positive_x_positive_z < min) {
            min = positive_x_positive_z;
            min_x = x + 1;
            min_z = z + 1;
        }

        float difference = x_z - min;

        if (payload > 0.1f) {
            position_data[min_x * world_size + min_z] += payload * 0.2f;
            payload *= 0.8f;
        }

        if (difference > 0 && difference < 1.0f && step < 20) {
//        if (step < 200) {
//            float amount = difference * 0.005f;
            float amount = difference * 0.005f * (1.0f - (float) step / 20.0f);

            position_data[(x - 1) * world_size + (z - 1)] -= amount / 4.0f;
            position_data[(x - 1) * world_size + (z)] -= amount / 2.0f;
            position_data[(x - 1) * world_size + (z + 1)] -= amount / 4.0f;
            position_data[(x) * world_size + (z - 1)] -= amount / 2.0f;
            position_data[(x) * world_size + (z)] -= amount;
            position_data[(x) * world_size + (z + 1)] -= amount / 2.0f;
            position_data[(x + 1) * world_size + (z - 1)] -= amount / 4.0f;
            position_data[(x + 1) * world_size + (z)] -= amount / 2.0f;
            position_data[(x + 1) * world_size + (z + 1)] -= amount / 4.0f;

            erode(position_data, min_x, min_z, payload + amount, step + 1);
        } else {
            position_data[min_x * world_size + min_z] += payload / 2.0f;
        }
    }
}

int main() {
    glfwInit();

    {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    }

    GLFWwindow *window = glfwCreateWindow(640, 480, "OpenGL", nullptr, nullptr);

    {
        glfwMakeContextCurrent(window);

        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        glfwSwapInterval(1);

        glfwGetWindowSize(window, &window_width, &window_height);

        glfwSetKeyCallback(window, key_callback);

//        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
//        glfwSetCursorPos(window, camera_angle.y / M_PI * (float) window_height, camera_angle.x / M_PI * (float) window_height);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_FRONT);
    }

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
    ImGui::StyleColorsDark();

    GLFWimage images[1];
    images[0].pixels = stbi_load("../res/images/icon_512.png", &images[0].width, &images[0].height, nullptr, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    Shader shader("../res/shaders/Basic.glsl");
    shader.bind();

    position_data_t position_data;

    for (int x = 0; x < world_size; x++) {
        for (int z = 0; z < world_size; z++) {
            position_data[x * world_size + z] = get_noise_value(x, z);
        }
    }

    std::vector<data_t> data_init;

    for (int x = 0; x < world_size - 1; x++) {
        for (int z = 0; z < world_size - 1; z++) {
            auto vertex_1 = glm::vec3(x, position_data[x * world_size + z], z);
            auto vertex_2 = glm::vec3(x, position_data[x * world_size + (z + 1)], z + 1);
            auto vertex_3 = glm::vec3(x + 1, position_data[(x + 1) * world_size + (z + 1)], z + 1);
            auto vertex_4 = glm::vec3(x, position_data[x * world_size + z], z);
            auto vertex_5 = glm::vec3(x + 1, position_data[(x + 1) * world_size + (z + 1)], z + 1);
            auto vertex_6 = glm::vec3(x + 1, position_data[(x + 1) * world_size + z], z);

            auto a = vertex_2 - vertex_1;
            auto b = vertex_3 - vertex_1;

            auto normal = glm::normalize(glm::cross(a, b));

            data_init.push_back(data_t{vertex_1, normal});
            data_init.push_back(data_t{vertex_2, normal});
            data_init.push_back(data_t{vertex_3, normal});
            data_init.push_back(data_t{vertex_4, normal});
            data_init.push_back(data_t{vertex_5, normal});
            data_init.push_back(data_t{vertex_6, normal});
        }
    }

    VertexArray vertex_array;

    VertexBufferLayout vertex_buffer_layout;
    vertex_buffer_layout.push(GL_FLOAT, 3, GL_FALSE);
    vertex_buffer_layout.push(GL_FLOAT, 3, GL_FALSE);
    VertexBuffer vertex_buffer(&data_init[0], data_init.size() * sizeof(data_t));

    vertex_array.add_buffer(vertex_buffer, vertex_buffer_layout);

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (!paused) {
            glfwGetCursorPos(window, &mx, &my);

            camera_angle.y = M_PI * ((float) mx / (float) window_height);
            camera_angle.x = M_PI * ((float) my / (float) window_height);

            if (keys[GLFW_KEY_W]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(0.0f, 0.0f, speed));
            }
            if (keys[GLFW_KEY_S]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(0.0f, 0.0f, -speed));
            }
            if (keys[GLFW_KEY_D]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(speed, 0.0f, 0.0f));
            }
            if (keys[GLFW_KEY_A]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(-speed, 0.0f, 0.0f));
            }
            if (keys[GLFW_KEY_SPACE]) {
                camera_position.y += speed;
            }
            if (keys[GLFW_KEY_LEFT_SHIFT]) {
                camera_position.y -= speed;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!paused) {
            for (int i = 0; i < 512; i++) {
                int x = glm::linearRand(0, world_size);
                int z = glm::linearRand(0, world_size);
                erode(position_data, x, z, 0.0f, 0);
            }
        }

        std::vector<data_t> data;

        for (int x = 0; x < world_size - 1; x++) {
            for (int z = 0; z < world_size - 1; z++) {
                auto vertex_1 = glm::vec3(x, position_data[x * world_size + z], z);
                auto vertex_2 = glm::vec3(x, position_data[x * world_size + (z + 1)], z + 1);
                auto vertex_3 = glm::vec3(x + 1, position_data[(x + 1) * world_size + (z + 1)], z + 1);
                auto vertex_4 = glm::vec3(x, position_data[x * world_size + z], z);
                auto vertex_5 = glm::vec3(x + 1, position_data[(x + 1) * world_size + (z + 1)], z + 1);
                auto vertex_6 = glm::vec3(x + 1, position_data[(x + 1) * world_size + z], z);

                auto a = vertex_2 - vertex_1;
                auto b = vertex_3 - vertex_1;

                auto normal = glm::normalize(glm::cross(a, b));

                data.push_back(data_t{vertex_1, normal});
                data.push_back(data_t{vertex_2, normal});
                data.push_back(data_t{vertex_3, normal});
                data.push_back(data_t{vertex_4, normal});
                data.push_back(data_t{vertex_5, normal});
                data.push_back(data_t{vertex_6, normal});
            }
        }

        vertex_buffer.set_data(&data[0], data.size() * sizeof(data_t));

        {
            glm::mat4 perspective = Transform::perspective_transformation(field_of_view, (float) window_width / (float) window_height, 0.01f, 1000.0f);
            glm::mat4 camera_rotate = Transform::rotate(camera_angle);
            glm::mat4 camera_translate = Transform::translate(glm::vec3(-camera_position.x, -camera_position.y, camera_position.z));

            shader.bind();
            shader.set_uniform_matrix_4fv("u_view_projection", perspective * camera_rotate * camera_translate);
            shader.set_uniform_3f("u_light_position", light_position);

            glDrawArrays(GL_TRIANGLES, 0, (GLsizei) data.size());
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::Begin("Debug");
            ImGui::SliderFloat("FOV", &field_of_view, 1.0f, 120.f);
            ImGui::SliderFloat("Speed", &speed, 0.1f, 10.f);
            ImGui::Text("%.3f ms/frame %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();

    return 0;
}
