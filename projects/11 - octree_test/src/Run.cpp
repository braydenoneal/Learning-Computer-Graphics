#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <map>
#include <glm/gtc/noise.hpp>
#include <stb_image.h>

#include "render/VertexArray.h"
#include "render/VertexBuffer.h"
#include "render/IndexBuffer.h"
#include "render/Shader.h"
#include "render/Transform.h"

int render_distance = 64;
int window_width, window_height;
auto camera_position = glm::vec3(render_distance / 2 * 16, 256.0f, -render_distance / 2 * 16);
auto camera_angle = glm::vec3(0.0f, 0.0f, 0.0f);
float field_of_view = 70.0f;
double mx, my;

std::map<int, int> keys;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
   keys[key] = action;

    if (key == GLFW_KEY_Q) {
        glfwTerminate();
        exit(0);
    }
}

std::vector<float> get_positions_at(int x, int y, int z) {
    return {
//             0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 0.8f,
//             0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z, 0.8f,
//             0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 0.8f,
//             0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.8f,
//             0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 0.8f,
//
//            -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 0.8f,
//            -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.8f,
//            -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 0.8f,
//            -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z, 0.8f,
//            -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 0.8f,

            -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 1.0f,
            -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 1.0f,
             0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 1.0f,
             0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 1.0f,
             0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 1.0f,

//             0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.4f,
//             0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z, 0.4f,
//            -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.4f,
//            -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z, 0.4f,
//            -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.4f,
//
//             0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 0.6f,
//             0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.6f,
//            -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 0.6f,
//            -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z, 0.6f,
//            -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z, 0.6f,
//
//            -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 0.6f,
//            -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z, 0.6f,
//             0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 0.6f,
//             0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z, 0.6f,
//             0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z, 0.6f,
    };
}

std::vector<float> get_chunk_positions_at(int cx, int cy, int cz) {
    std::vector<float> positions{};

    for (int x = 0; x < 16; x++) {
        for (int z = 0; z < 16; z++) {
            auto px = (float) (cx * 16 + x);
            auto pz = (float) (cz * 16 + z);

            float level1 = glm::perlin(glm::vec2(px / 32.0f, pz / 32.0f));
            float level2 = 2 * glm::perlin(glm::vec2(px / 256.0f, pz / 256.0f));

            int y = std::floor((float) 128 * 0.5f / 2.0f * (1.0f + level1 + level2));

            std::vector<float> block_positions = get_positions_at(x + cx * 16, y, z + cz * 16);
            positions.insert(positions.end(), block_positions.begin(), block_positions.end());
        }
    }

    return positions;
}

std::vector<float> get_all_chunk_positions() {
    std::vector<float> positions{};

    for (int x = 0; x < render_distance; x++) {
        for (int z = 0; z < render_distance; z++) {
            std::vector<float> block_positions = get_chunk_positions_at(x, 0, z);
            positions.insert(positions.end(), block_positions.begin(), block_positions.end());
        }
    }

    return positions;
}

static std::vector<unsigned int> get_indices_of_size(unsigned long long faces_count) {
    std::vector<unsigned int> indices{};

    for (int i = 0; i < faces_count; i++) {
        indices.push_back(i * 5 + 0);
        indices.push_back(i * 5 + 1);
        indices.push_back(i * 5 + 2);
        indices.push_back(i * 5 + 3);
        indices.push_back(i * 5 + 4);
        indices.push_back(i * 5 + 1);
    }

    return indices;
}

int main() {
    glfwInit();

    {

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);
    }

    GLFWwindow *window = glfwCreateWindow(640, 480, "Minecraft", nullptr, nullptr);

    {
        glfwMakeContextCurrent(window);

        gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

        glfwSwapInterval(1);

        glfwGetWindowSize(window, &window_width, &window_height);

        glfwSetKeyCallback(window, key_callback);

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        glfwSetCursorPos(window, 0, 0);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
    }

    GLFWimage images[1];
    images[0].pixels = stbi_load("../res/images/icon_512.png", &images[0].width, &images[0].height, nullptr, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    VertexBufferLayout vertex_buffer_layout;
    Shader shader("../res/shaders/Basic.glsl");
    shader.bind();

    vertex_buffer_layout.push(GL_FLOAT, 3, GL_FALSE);
    vertex_buffer_layout.push(GL_FLOAT, 1, GL_FALSE);

    std::vector<float> all_chunk_positions = get_all_chunk_positions();
    std::vector<unsigned int> chunk_indices = get_indices_of_size(all_chunk_positions.size());
    VertexArray vertex_array;
    VertexBuffer vertex_buffer(all_chunk_positions.data(), all_chunk_positions.size() * sizeof(float));
    vertex_array.add_buffer(vertex_buffer, vertex_buffer_layout);
    IndexBuffer index_buffer(chunk_indices.data(), chunk_indices.size());

    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        {
            glfwGetCursorPos(window, &mx, &my);

            camera_angle.y = M_PI * ((float) mx / (float) window_height);
            camera_angle.x = M_PI * ((float) my / (float) window_height);

            if (keys[GLFW_KEY_W]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(0.0f, 0.0f, 5.0f));
            }
            if (keys[GLFW_KEY_S]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(0.0f, 0.0f, -5.0f));
            }
            if (keys[GLFW_KEY_D]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(5.0f, 0.0f, 0.0f));
            }
            if (keys[GLFW_KEY_A]) {
                camera_position = Transform::translate_in_direction_by_amount(camera_position, camera_angle.y,
                                                                              glm::vec3(-5.0f, 0.0f, 0.0f));
            }
            if (keys[GLFW_KEY_SPACE]) {
                camera_position.y += 5.0f;
            }
            if (keys[GLFW_KEY_LEFT_SHIFT]) {
                camera_position.y -= 5.0f;
            }
        }

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        {
            glm::mat4 perspective = Transform::perspective_transformation(field_of_view, (float) window_width / (float) window_height, 0.01f, 10000.0f);
            glm::mat4 camera_rotate = Transform::rotate(camera_angle);
            glm::mat4 camera_translate = Transform::translate(glm::vec3(-camera_position.x, -camera_position.y, camera_position.z));

            shader.bind();
            shader.set_uniform_matrix_4fv("u_view_projection", perspective * camera_rotate * camera_translate);

            glDrawElements(GL_TRIANGLES, (GLsizei) index_buffer.get_count(), GL_UNSIGNED_INT, nullptr);
        }

        glfwSwapBuffers(window);
    }

    glfwTerminate();

    return 0;
}
