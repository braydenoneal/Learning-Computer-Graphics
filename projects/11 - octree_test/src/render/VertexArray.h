#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "VertexBuffer.h"

class VertexArray {
public:
    unsigned int renderer_id{};

    VertexArray() {
        glGenVertexArrays(1, &renderer_id);
        glBindVertexArray(renderer_id);
    }

    ~VertexArray() {
        glDeleteVertexArrays(1, &renderer_id);
    }

    void bind() const {
        glBindVertexArray(renderer_id);
    }

    static void unbind() {
        glBindVertexArray(0);
    }

    void add_buffer(const VertexBuffer &vertex_buffer, const VertexBufferLayout &vertex_buffer_layout) const {
        bind();
        vertex_buffer.bind();
        const auto &elements = vertex_buffer_layout.get_elements();
        unsigned int offset = 0;

        for (unsigned int i = 0; i < elements.size(); i++) {
            const auto &element = elements[i];
            glEnableVertexAttribArray(i);
            glVertexAttribPointer(i, element.count, element.type, element.normalized, vertex_buffer_layout.get_stride(),
                                  (const void *) (intptr_t) offset);
            offset += element.count * VertexBufferElement::get_size_of_type(element.type);
        }
    }
};
