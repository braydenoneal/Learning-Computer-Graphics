#pragma once

#include "../buffers/VertexArray.h"
#include "../buffers/IndexBuffer.h"
#include "../shaders/Shader.h"

class Renderer {
public:
    void clear() const;

    void Draw(const VertexArray &va, const IndexBuffer &ib, const Shader &shader) const;
};
