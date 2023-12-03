#include <vector>
#include <array>
#include <algorithm>
#include <glm/gtc/noise.hpp>

const unsigned int chunk_size = 16;
const unsigned int chunk_height = 96;

struct AmbientMap {
    bool px_py_pz = false;
    bool px_py_nz = false;
    bool nx_py_pz = false;
    bool nx_py_nz = false;
    bool px_ny_pz = false;
    bool px_ny_nz = false;
    bool nx_ny_pz = false;
    bool nx_ny_nz = false;
};

class ChunkData {
public:
    typedef unsigned int block_id_t;
    typedef std::array<block_id_t, chunk_size * chunk_height * chunk_size> chunk_data_t;

    typedef std::array<float, 7> vertex_t;
    typedef std::array<vertex_t, 5> face_t;

    typedef std::vector<face_t> mesh_data_t;

    static unsigned int get_chunk_position(unsigned int x, unsigned int y, unsigned int z) {
        return x * chunk_size * chunk_height + y * chunk_size + z;
    }

    static chunk_data_t get_flat_chunk_data() {
        chunk_data_t chunk_data{0};

        for (int x = 0; x < chunk_size; x++) {
            for (int z = 0; z < chunk_size; z++) {
                chunk_data[get_chunk_position(x, 4, z)] = 1;
            }
        }

        return chunk_data;
    }

    static chunk_data_t get_perlin_chunk_data(int cx, int cz) {
        chunk_data_t chunk_data{0};

        for (int x = 0; x < chunk_size; x++) {
            for (int z = 0; z < chunk_size; z++) {
                auto px = (float) (cx * chunk_size + x);
                auto pz = (float) (cz * chunk_size + z);

                float level0 = glm::perlin(glm::vec2(px / 128.0f, pz / 128.0f));
                float level1 = 0.5f * glm::perlin(glm::vec2(px / 16.0f, pz / 16.0f));

                int y = std::floor((float) chunk_height * 0.5f / 2.0f * (1.0f + level1));

                chunk_data[get_chunk_position(x, y, z)] = 1;

                int sy = std::floor((float) y * 0.98f);

                for (int i = 0 ; i < sy; i++) {
                    chunk_data[get_chunk_position(x, i, z)] = 3;
                }

                for (int i = sy ; i < y; i++) {
                    chunk_data[get_chunk_position(x, i, z)] = 2;
                }
            }
        }

        return chunk_data;
    }

    static std::vector<chunk_data_t> get_flat_chunks(int size) {
        std::vector<chunk_data_t> chunk_data{};

        for (int x = 0; x < size; x++) {
            for (int z = 0; z < size; z++) {
                chunk_data.push_back(get_perlin_chunk_data(x, z));
            }
        }

        return chunk_data;
    }

    static face_t get_positive_x_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  0.0f, (ambient_map.px_py_nz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  0.0f, (ambient_map.px_ny_nz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.px_py_pz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  0.0f, (ambient_map.px_ny_pz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.px_py_pz ? 0.6f : 0.8f) },
        };
    }

    static face_t get_negative_x_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  0.0f, (ambient_map.nx_py_pz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  0.0f, (ambient_map.nx_ny_pz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.nx_py_nz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  0.0f, (ambient_map.nx_ny_nz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.nx_py_nz ? 0.6f : 0.8f) },
        };
    }

    static face_t get_positive_y_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  1.0f, (ambient_map.nx_py_pz ? 0.8f : 1.0f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  1.0f, (ambient_map.nx_py_nz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  1.0f, (ambient_map.px_py_pz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  1.0f, (ambient_map.px_py_nz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  1.0f, (ambient_map.px_py_pz ? 0.8f : 1.0f) },
        };
    }

    static face_t get_negative_y_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.px_ny_pz ? 0.2f : 0.4f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.px_ny_nz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_ny_pz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.nx_ny_nz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_ny_pz ? 0.2f : 0.4f) },
        };
    }

    static face_t get_positive_z_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  0.0f, (ambient_map.px_py_pz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  0.0f, (ambient_map.px_ny_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.nx_py_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  0.0f, (ambient_map.nx_ny_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.nx_py_pz ? 0.4f : 0.6f) },
        };
    }

    static face_t get_negative_z_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  0.0f, (ambient_map.nx_py_nz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  0.0f, (ambient_map.nx_ny_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.px_py_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  0.0f, (ambient_map.px_ny_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f, (ambient_map.px_py_nz ? 0.4f : 0.6f) },
        };
    }

    static face_t get_positive_x_dirt_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  2.0f, (ambient_map.px_py_nz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  2.0f, (ambient_map.px_ny_nz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.px_py_pz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  2.0f, (ambient_map.px_ny_pz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.px_py_pz ? 0.6f : 0.8f) },
        };
    }

    static face_t get_negative_x_dirt_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  2.0f, (ambient_map.nx_py_pz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  2.0f, (ambient_map.nx_ny_pz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.nx_py_nz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  2.0f, (ambient_map.nx_ny_nz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.nx_py_nz ? 0.6f : 0.8f) },
        };
    }

    static face_t get_positive_y_dirt_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  2.0f, (ambient_map.nx_py_pz ? 0.8f : 1.0f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  2.0f, (ambient_map.nx_py_nz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.px_py_pz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  2.0f, (ambient_map.px_py_nz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.px_py_pz ? 0.8f : 1.0f) },
        };
    }

    static face_t get_negative_y_dirt_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  2.0f, (ambient_map.px_ny_pz ? 0.2f : 0.4f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  2.0f, (ambient_map.px_ny_nz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.nx_ny_pz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  2.0f, (ambient_map.nx_ny_nz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.nx_ny_pz ? 0.2f : 0.4f) },
        };
    }

    static face_t get_positive_z_dirt_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  2.0f, (ambient_map.px_py_pz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  2.0f, (ambient_map.px_ny_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.nx_py_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  2.0f, (ambient_map.nx_ny_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.nx_py_pz ? 0.4f : 0.6f) },
        };
    }

    static face_t get_negative_z_dirt_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  2.0f, (ambient_map.nx_py_nz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  2.0f, (ambient_map.nx_ny_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.px_py_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  2.0f, (ambient_map.px_ny_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  2.0f, (ambient_map.px_py_nz ? 0.4f : 0.6f) },
        };
    }

    static face_t get_positive_x_stone_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.px_py_nz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.px_ny_nz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.px_py_pz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.px_ny_pz ? 0.6f : 0.8f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.px_py_pz ? 0.6f : 0.8f) },
        };
    }

    static face_t get_negative_x_stone_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.nx_py_pz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.nx_ny_pz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_py_nz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.nx_ny_nz ? 0.6f : 0.8f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_py_nz ? 0.6f : 0.8f) },
        };
    }

    static face_t get_positive_y_stone_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.nx_py_pz ? 0.8f : 1.0f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.nx_py_nz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.px_py_pz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.px_py_nz ? 0.8f : 1.0f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.px_py_pz ? 0.8f : 1.0f) },
        };
    }

    static face_t get_negative_y_stone_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.px_ny_pz ? 0.2f : 0.4f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.px_ny_nz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_ny_pz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.nx_ny_nz ? 0.2f : 0.4f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_ny_pz ? 0.2f : 0.4f) },
        };
    }

    static face_t get_positive_z_stone_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.px_py_pz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.px_ny_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_py_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.nx_ny_pz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.nx_py_pz ? 0.4f : 0.6f) },
        };
    }

    static face_t get_negative_z_stone_face(int x, int y, int z, AmbientMap ambient_map) {
        return {
                vertex_t { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  3.0f, (ambient_map.nx_py_nz ? 0.4f : 0.6f) },
                vertex_t { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  3.0f, (ambient_map.nx_ny_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.px_py_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  3.0f, (ambient_map.px_ny_nz ? 0.4f : 0.6f) },
                vertex_t {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  3.0f, (ambient_map.px_py_nz ? 0.4f : 0.6f) },
        };
    }

    static mesh_data_t get_chunk_mesh(chunk_data_t chunk_data) {
        mesh_data_t mesh_data{};

        for (int x = 0; x < chunk_size; x++) {
            for (int y = 0; y < chunk_height; y++) {
                for (int z = 0; z < chunk_size; z++) {
                    AmbientMap ambient_map{};
                    if (chunk_data[get_chunk_position(x, y, z)] != 0) {
                        if (x > 0 && x < chunk_size - 1 && y > 0 && y < chunk_height - 1 && z > 0 && z < chunk_size - 1) {
                            ambient_map = {
                                chunk_data[get_chunk_position(x + 1, y + 1, z + 1)] > 0,
                                chunk_data[get_chunk_position(x + 1, y + 1, z - 1)] > 0,
                                chunk_data[get_chunk_position(x - 1, y + 1, z + 1)] > 0,
                                chunk_data[get_chunk_position(x - 1, y + 1, z - 1)] > 0,
                                chunk_data[get_chunk_position(x + 1, y - 1, z + 1)] > 0,
                                chunk_data[get_chunk_position(x + 1, y - 1, z - 1)] > 0,
                                chunk_data[get_chunk_position(x - 1, y - 1, z + 1)] > 0,
                                chunk_data[get_chunk_position(x - 1, y - 1, z - 1)] > 0,
                            };
                        }
                    }

                    if (chunk_data[get_chunk_position(x, y, z)] == 1) {
                        if (x == chunk_size - 1 || (x < chunk_size - 1 && chunk_data[get_chunk_position(x + 1, y, z)] == 0)) {
                            mesh_data.push_back(get_positive_x_face(x, y, z, ambient_map));
                        }
                        if (x == 0 || (x > 0 && chunk_data[get_chunk_position(x - 1, y, z)] == 0))
                            mesh_data.push_back(get_negative_x_face(x, y, z, ambient_map));
                        if (y == chunk_size - 1 || (y < chunk_height - 1 && chunk_data[get_chunk_position(x, y + 1, z)] == 0)) {
                            mesh_data.push_back(get_positive_y_face(x, y, z, ambient_map));
                        }
                        if (y == 0 || (y > 0 && chunk_data[get_chunk_position(x, y - 1, z)] == 0))
                            mesh_data.push_back(get_negative_y_face(x, y, z, ambient_map));
                        if (z == chunk_size - 1 || (z < chunk_size - 1 && chunk_data[get_chunk_position(x, y, z + 1)] == 0)) {
                            mesh_data.push_back(get_positive_z_face(x, y, z, ambient_map));
                        }
                        if (z == 0 || (z > 0 && chunk_data[get_chunk_position(x, y, z - 1)] == 0)) {
                            mesh_data.push_back(get_negative_z_face(x, y, z, ambient_map));
                        }
                    } else if (chunk_data[get_chunk_position(x, y, z)] == 2) {
                        if (x == chunk_size - 1 || (x < chunk_size - 1 && chunk_data[get_chunk_position(x + 1, y, z)] == 0)) {
                            mesh_data.push_back(get_positive_x_dirt_face(x, y, z, ambient_map));
                        }
                        if (x == 0 || (x > 0 && chunk_data[get_chunk_position(x - 1, y, z)] == 0))
                            mesh_data.push_back(get_negative_x_dirt_face(x, y, z, ambient_map));
                        if (y == chunk_size - 1 || (y < chunk_height - 1 && chunk_data[get_chunk_position(x, y + 1, z)] == 0)) {
                            mesh_data.push_back(get_positive_y_dirt_face(x, y, z, ambient_map));
                        }
                        if (y == 0 || (y > 0 && chunk_data[get_chunk_position(x, y - 1, z)] == 0))
                            mesh_data.push_back(get_negative_y_dirt_face(x, y, z, ambient_map));
                        if (z == chunk_size - 1 || (z < chunk_size - 1 && chunk_data[get_chunk_position(x, y, z + 1)] == 0)) {
                            mesh_data.push_back(get_positive_z_dirt_face(x, y, z, ambient_map));
                        }
                        if (z == 0 || (z > 0 && chunk_data[get_chunk_position(x, y, z - 1)] == 0)) {
                            mesh_data.push_back(get_negative_z_dirt_face(x, y, z, ambient_map));
                        }
                    } else if (chunk_data[get_chunk_position(x, y, z)] == 3) {
                        if (x == chunk_size - 1 || (x < chunk_size - 1 && chunk_data[get_chunk_position(x + 1, y, z)] == 0)) {
                            mesh_data.push_back(get_positive_x_stone_face(x, y, z, ambient_map));
                        }
                        if (x == 0 || (x > 0 && chunk_data[get_chunk_position(x - 1, y, z)] == 0))
                            mesh_data.push_back(get_negative_x_stone_face(x, y, z, ambient_map));
                        if (y == chunk_size - 1 || (y < chunk_height - 1 && chunk_data[get_chunk_position(x, y + 1, z)] == 0)) {
                            mesh_data.push_back(get_positive_y_stone_face(x, y, z, ambient_map));
                        }
                        if (y == 0 || (y > 0 && chunk_data[get_chunk_position(x, y - 1, z)] == 0))
                            mesh_data.push_back(get_negative_y_stone_face(x, y, z, ambient_map));
                        if (z == chunk_size - 1 || (z < chunk_size - 1 && chunk_data[get_chunk_position(x, y, z + 1)] == 0)) {
                            mesh_data.push_back(get_positive_z_stone_face(x, y, z, ambient_map));
                        }
                        if (z == 0 || (z > 0 && chunk_data[get_chunk_position(x, y, z - 1)] == 0)) {
                            mesh_data.push_back(get_negative_z_stone_face(x, y, z, ambient_map));
                        }
                    }
                }
            }
        }

        return mesh_data;
    }

    static std::vector<mesh_data_t> get_chunk_meshes(const std::vector<chunk_data_t> &chunk_datas) {
        std::vector<mesh_data_t> chunk_meshes{};

        for (chunk_data_t chunk_data : chunk_datas) {
            chunk_meshes.push_back(get_chunk_mesh(chunk_data));
        }

        return chunk_meshes;
    }

    static std::vector<unsigned int> get_indices_of_size(unsigned long long faces_count) {
        std::vector<unsigned int> indices{};

        for (int i = 0; i < faces_count; i++) {
            indices.push_back(i * 5 + 0);
            indices.push_back(i * 5 + 1);
            indices.push_back(i * 5 + 2);
            indices.push_back(i * 5 + 3);
            indices.push_back(i * 5 + 4);
            indices.push_back(i * 5 + 1);
        }

        return indices;
    }
};
