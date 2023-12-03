#include "VertexBufferLayout.h"

class VertexBuffer {
public:
    unsigned int renderer_id{};

    VertexBuffer(const void *data, unsigned int size) {
        glGenBuffers(1, &renderer_id);
        glBindBuffer(GL_ARRAY_BUFFER, renderer_id);
        glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
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
};
