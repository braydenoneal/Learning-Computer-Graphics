#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "VertexBufferLayout.h"

class VertexBuffer {
public:
    unsigned int renderer_id{};

    VertexBuffer(const void *data, unsigned int size) {
        glGenBuffers(1, &renderer_id);
        glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
        glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }

    ~VertexBuffer() {
        glDeleteBuffers(1, &renderer_id);
    }

    void bind() const {
        glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
    }

    static void unbind() {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void set_data(const void *data, unsigned int size) const {
        glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
        glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
    }
};
