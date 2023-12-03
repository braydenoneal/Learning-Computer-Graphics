struct UniformsPackage {
    glm::mat4 perspective;
    glm::vec3 camera_position;
    glm::vec3 camera_angle;
};

class DrawChunk {
public:
    std::vector<std::string> texture_paths = {"../res/textures/grass_block_side_beta.png", "../res/textures/grass_block_top_beta.png", "../res/textures/dirt.png", "../res/textures/stone.png"};
    VertexBufferLayout layout = VertexBufferLayout();
    Shader shader = Shader("../res/shaders/Basic2.glsl");
    Texture textures = Texture(texture_paths);

    DrawChunk() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        layout.push(GL_FLOAT, 3, GL_FALSE);
        layout.push(GL_FLOAT, 3, GL_FALSE);
        layout.push(GL_FLOAT, 1, GL_FALSE);

        textures.bind(0);
        shader.set_uniform_1i("u_Textures", 0);
    }

    void draw(const ChunkData::mesh_data_t &mesh_data, int chunk_x, int chunk_z) {
        VertexArray vertex_array;
        VertexBuffer vertex_buffer(mesh_data.data(), mesh_data.size() * 5 * 7 * sizeof(float));
        IndexBuffer index_buffer(ChunkData::get_indices_of_size(mesh_data.size()).data(), mesh_data.size() * 6);
        vertex_array.add_buffer(vertex_buffer, layout);

        glm::mat4 model_translate = Transform::translate(glm::vec3(((float) chunk_x) * 16, 0.0f, ((float) chunk_z) * 16));
        shader.set_uniform_matrix_4fv("u_mt", model_translate);

        glDrawElements(GL_TRIANGLES, (GLsizei) index_buffer.get_count(), GL_UNSIGNED_INT, nullptr);
    }

    void draw_all(UniformsPackage &uniforms_package, const std::vector<ChunkData::mesh_data_t> &mesh_datas, int count) {
        glm::mat4 camera_rotate = Transform::rotate(uniforms_package.camera_angle);
        glm::mat4 camera_translate = Transform::translate(glm::vec3(-uniforms_package.camera_position.x, -uniforms_package.camera_position.y, uniforms_package.camera_position.z));

        shader.bind();
        shader.set_uniform_matrix_4fv("u_all", uniforms_package.perspective * camera_rotate * camera_translate);

        for (int x = 0; x < count; x++) {
            for (int z = 0; z < count; z++) {
                draw(mesh_datas[x * count + z], x - count / 2, z - count / 2);
            }
        }
    }
};
