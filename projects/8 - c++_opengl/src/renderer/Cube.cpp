#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <iostream>

#include "Cube.h"
#include "../buffers/VertexBufferLayout.h"
#include "../textures/Texture.h"
#include "Renderer.h"

void Cube::draw(Renderer renderer, glm::mat4 mvp) {
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

    VertexArray va = VertexArray();
    VertexBuffer vb(positions, 30 * 7 * sizeof(float));

    VertexBufferLayout layout;
    layout.Push(GL_FLOAT, 3, GL_FALSE);
    layout.Push(GL_FLOAT, 3, GL_FALSE);
    layout.Push(GL_FLOAT, 1, GL_FALSE);
    va.AddBuffer(vb, layout);

    IndexBuffer ib = IndexBuffer(indices, tris_count * 3);

    Shader shader = Shader("../res/shaders/Basic.glsl");
    shader.Bind();
    shader.SetUniformMatrix4fv("u_MVP", mvp);

    std::vector<std::string> texture_paths = {"../res/textures/grass_block_side.png", "../res/textures/grass_block_top.png", "../res/textures/dirt.png"};

    Texture textures(texture_paths);
    textures.Bind(0);
    shader.SetUniform1i("u_Textures", 0);

    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();

    renderer.Draw(va, ib, shader);
}
