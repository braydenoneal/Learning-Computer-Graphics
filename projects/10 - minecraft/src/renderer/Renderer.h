class Renderer {
public:
    WindowContext *window_context;
    GameContext *game_context;

    Renderer(WindowContext *input_window_context, GameContext *input_game_context) {
        window_context = input_window_context;
        game_context = input_game_context;
    }

    static void draw(const VertexArray &vertex_array, const IndexBuffer &index_buffer, const Shader &shader) {
        shader.bind();
        vertex_array.bind();
        index_buffer.bind();
        glDrawElements(GL_TRIANGLES, index_buffer.get_count(), GL_UNSIGNED_INT, nullptr);
    };

    void render() {
        clear();
    }

    static void clear() {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(141.0f / 255.0f, 159.0f / 255.0f, 1.0f, 1.0f);
    }
};
