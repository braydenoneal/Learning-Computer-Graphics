#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <sstream>

#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 800, "Test", nullptr, nullptr);
    if (window == nullptr) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
 
    glfwMakeContextCurrent(window);

    glfwSwapInterval(1);
 
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    {
        float positions[] = {
            -0.5f, -0.5f,
            0.5f, -0.5f,
            0.5f,  0.5f,
            -0.5f,  0.5f
        };

        unsigned int indices[] = {
            0, 1, 2,
            2, 3, 0
        };

        VertexArray va;
        VertexBuffer vb(positions, 4 * 2 * sizeof(float));

        VertexBufferLayout layout;
        layout.Push(GL_FLOAT, 2, GL_FALSE);
        va.AddBuffer(vb, layout);

        IndexBuffer ib(indices, 6);

        Shader shader("../res/shaders/Basic.glsl");
        shader.Bind();
        shader.SetUniform4f("u_Color", 1.0f, 0.0f, 0.0f, 1.0f);

        va.Unbind();
        vb.Unbind();
        ib.Unbind();
        shader.Unbind();
    
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
    
            glClear(GL_COLOR_BUFFER_BIT);
            
            shader.Bind();
            shader.SetUniform4f("u_Color", 1.0f, 0.0f, 0.0f, 1.0f);

            va.Bind();
            ib.Bind();

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    
            glfwSwapBuffers(window);
        }
    }
 
    glfwTerminate();
    return 0;
}
