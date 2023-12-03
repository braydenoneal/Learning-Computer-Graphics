#pragma once

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <vector>

struct VertexBufferElement {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    static unsigned int get_size_of_type(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:
                return 1;
            default:
                return 0;
        }
    }
};

class VertexBufferLayout {
public:
    std::vector<VertexBufferElement> elements;
    unsigned int stride{0};

    void push(unsigned int type, unsigned int count, unsigned char normalized) {
        elements.push_back({type, count, normalized});
        stride += VertexBufferElement::get_size_of_type(type) * count;
    }

    [[nodiscard]] std::vector<VertexBufferElement> get_elements() const {
        return elements;
    }

    [[nodiscard]] unsigned int get_stride() const {
        return stride;
    }
};
