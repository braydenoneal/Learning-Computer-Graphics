#pragma once

#include <vector>
#include "../buffers/VertexArray.h"
#include "../buffers/IndexBuffer.h"
#include "../shaders/Shader.h"
#include "Renderer.h"

class Cube {
public:
    static void draw(Renderer renderer, glm::mat4 mvp);
};
