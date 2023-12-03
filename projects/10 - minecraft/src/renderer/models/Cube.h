#include <glm/gtc/noise.hpp>

class Cube {
public:
    std::vector<std::string> texture_paths = {"../res/textures/grass_block_side.png", "../res/textures/grass_block_top.png", "../res/textures/dirt.png"};

    VertexArray va = VertexArray();
    VertexBufferLayout layout = VertexBufferLayout();
    VertexBuffer vb;
    IndexBuffer ib;
    Shader shader = Shader("../res/shaders/Basic.glsl");
    Texture textures = Texture(texture_paths);

    Cube(int quad_count, float positions[], unsigned int indices[]) :
            vb(VertexBuffer(positions, quad_count * 5 * 7 * sizeof(float))),
            ib(IndexBuffer(indices, quad_count * 6)) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        layout.push(GL_FLOAT, 3, GL_FALSE);
        layout.push(GL_FLOAT, 3, GL_FALSE);
        layout.push(GL_FLOAT, 1, GL_FALSE);
        va.add_buffer(vb, layout);

        textures.bind(0);
        shader.set_uniform_1i("u_Textures", 0);
    }

    void draw(glm::mat4 perspective, unsigned int count, glm::vec3 camera_position, glm::vec3 camera_angle, glm::vec3 model_position, glm::vec3 model_angle) {
        glm::mat4 camera_rotate = Transform::rotate(camera_angle);
        glm::mat4 camera_translate = Transform::translate(glm::vec3(-camera_position.x, -camera_position.y, camera_position.z));
        glm::mat4 model_rotate = Transform::rotate(model_angle);

        shader.bind();
        shader.set_uniform_matrix_4fv("u_all", perspective * camera_rotate * camera_translate);

        for (unsigned int i = 0; i < count; i++) {
            for (unsigned int j = 0; j < count; j++) {
//                float y = roundf(Chunk::size * 3 * glm::perlin(glm::vec2((float) i / Chunk::size, (float) j / Chunk::size)));
//                glm::mat4 model_translate = Transform::translate(glm::vec3(model_position.x + ((float) i - (float) count / 2.0f) * Chunk::size, model_position.y + y, model_position.z + ((float) j - (float) count / 2.0f) * Chunk::size));
                glm::mat4 model_translate = Transform::translate(glm::vec3(model_position.x + ((float) i - (float) count / 2.0f) * Chunk::size, model_position.y, model_position.z + ((float) j - (float) count / 2.0f) * Chunk::size));
                shader.set_uniform_matrix_4fv("u_mt", model_translate);

                glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);
            }
        }
    }

    void static_draw(glm::mat4 perspective, glm::vec3 camera_position, glm::vec3 camera_angle, glm::vec3 model_position, glm::vec3 model_angle, int chunk_x, int chunk_z) {
//        glEnable(GL_BLEND);
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(GL_LESS);
//
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_FRONT);

//        VertexArray va_1 = VertexArray();
//        VertexBufferLayout layout = VertexBufferLayout();
//        VertexBuffer vb_1 = VertexBuffer(&positions[0], quad_count * 5 * 7 * sizeof(float));
//        IndexBuffer ib_1 = IndexBuffer(&indices[0], quad_count * 6);
//        Shader shader_1 = Shader("../res/shaders/Basic.glsl");
//        std::vector<std::string> texture_paths = {"../res/textures/grass_block_side.png", "../res/textures/grass_block_top.png", "../res/textures/dirt.png"};
//        Texture textures_1 = Texture(texture_paths);

//        layout.push(GL_FLOAT, 3, GL_FALSE);
//        layout.push(GL_FLOAT, 3, GL_FALSE);
//        layout.push(GL_FLOAT, 1, GL_FALSE);
//        va_1.add_buffer(vb_1, layout);

//        textures_1.bind(0);
//        shader_1.set_uniform_1i("u_Textures", 0);

        glm::mat4 camera_rotate = Transform::rotate(camera_angle);
        glm::mat4 camera_translate = Transform::translate(glm::vec3(-camera_position.x, -camera_position.y, camera_position.z));
        glm::mat4 model_rotate = Transform::rotate(model_angle);

        shader.bind();
        shader.set_uniform_matrix_4fv("u_all", perspective * camera_rotate * camera_translate);

        glm::mat4 model_translate = Transform::translate(glm::vec3(model_position.x + ((float) chunk_x) * Chunk::size, model_position.y, model_position.z + ((float) chunk_z) * Chunk::size));
        shader.set_uniform_matrix_4fv("u_mt", model_translate);

        glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);
    }
};
