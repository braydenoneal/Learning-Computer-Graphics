class IndexBuffer {
public:
    unsigned int renderer_id{};
    unsigned int count;

    IndexBuffer(const unsigned int *data, unsigned int input_count) {
        count = input_count;
        glGenBuffers(1, &renderer_id);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), data, GL_STATIC_DRAW);
    }

    ~IndexBuffer() {
        glDeleteBuffers(1, &renderer_id);
    }

    void bind() const {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_id);
    }

    static void unbind() {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    [[nodiscard]] unsigned int get_count() const {
        return count;
    }
};
