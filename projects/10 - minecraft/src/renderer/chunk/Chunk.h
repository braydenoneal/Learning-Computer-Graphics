#include <array>
#include <random>
#include <glm/gtc/noise.hpp>

struct BlockFaces {
    unsigned int block;
    unsigned int x;
    unsigned int y;
    unsigned int z;
    bool positive_x;
    bool negative_x;
    bool positive_y;
    bool negative_y;
    bool positive_z;
    bool negative_z;
};

struct Vertex {
    float x;
    float y;
    float z;
    float s;
    float t;
    float texture;
    float brightness;
};

struct Face {
    Vertex vertex0;
    Vertex vertex1;
    Vertex vertex2;
    Vertex vertex3;
    Vertex vertex4;
};

class Chunk {
public:
    static std::array<std::array<std::array<unsigned int, 16>, 16>, 16> flat_chunk_blocks(int cx, int cz) {
        std::array<std::array<std::array<unsigned int, 16>, 16>, 16> blocks{0};

        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                blocks[x][8 + (int) roundf(size / 4 * glm::perlin(glm::vec2((float) (x + cx * size) / size, (float) (z + cz * size) / size)))][z] = 1;
            }
        }

        return blocks;
    }

    static std::array<std::array<std::array<BlockFaces, 16>, 16>, 16> get_block_faces(std::array<std::array<std::array<unsigned int, 16>, 16>, 16> chunk_blocks) {
        std::array<std::array<std::array<BlockFaces, 16>, 16>, 16> block_faces{};

        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                for (int z = 0; z < 16; z++) {
                    if (chunk_blocks[x][y][z] == 1) {
                        bool positive_x = (x == 15 || (x < 15 && chunk_blocks[x + 1][y][z] != 1));
                        bool negative_x = (x == 0 || (x > 0 && chunk_blocks[x - 1][y][z] != 1));
                        bool positive_y = (y == 15 || (y < 15 && chunk_blocks[x][y + 1][z] != 1));
                        bool negative_y = (y == 0 || (y > 0 && chunk_blocks[x][y - 1][z] != 1));
                        bool positive_z = (z == 15 || (z < 15 && chunk_blocks[x][y][z + 1] != 1));
                        bool negative_z = (z == 0 || (z > 0 && chunk_blocks[x][y][z - 1] != 1));

                        block_faces[x][y][z] = {
                                chunk_blocks[x][y][z], (unsigned int) x, (unsigned int) y, (unsigned int) z,
                                positive_x, negative_x, positive_y, negative_y, positive_z, negative_z
                        };
                    }
                }
            }
        }

        return block_faces;
    }

    static Face get_positive_x_face(int x, int y, int z) {
        return {
                { 0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  0.0f,  0.8f },
                { 0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  0.0f,  0.8f },
                { 0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.8f },
                { 0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  0.0f,  0.8f },
                { 0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.8f },
        };
    }

    static Face get_negative_x_face(int x, int y, int z) {
        return {
                { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  0.0f,  0.8f },
                { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  0.0f,  0.8f },
                { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.8f },
                { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  0.0f,  0.8f },
                { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.8f },
        };
    }

    static Face get_positive_y_face(int x, int y, int z) {
        return {
                { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  1.0f,  1.0f },
                { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  1.0f,  1.0f },
                {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  1.0f,  1.0f },
                {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  1.0f,  1.0f },
                {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  1.0f,  1.0f },
        };
    }

    static Face get_negative_y_face(int x, int y, int z) {
        return {
                {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  2.0f,  0.4f },
                {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  2.0f,  0.4f },
                { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f,  0.4f },
                { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  2.0f,  0.4f },
                { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  2.0f,  0.4f },
        };
    }

    static Face get_positive_z_face(int x, int y, int z) {
        return {
                {  0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  0.0f,  1.0f,  0.0f,  0.6f },
                {  0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  0.0f,  0.0f,  0.0f,  0.6f },
                { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.6f },
                { -0.5f + (float) x, -0.5f + (float) y,  0.5f + (float) z,  1.0f,  0.0f,  0.0f,  0.6f },
                { -0.5f + (float) x,  0.5f + (float) y,  0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.6f },
        };
    }

    static Face get_negative_z_face(int x, int y, int z) {
        return {
                { -0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  0.0f,  1.0f,  0.0f,  0.6f },
                { -0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  0.0f,  0.0f,  0.0f,  0.6f },
                {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.6f },
                {  0.5f + (float) x, -0.5f + (float) y, -0.5f + (float) z,  1.0f,  0.0f,  0.0f,  0.6f },
                {  0.5f + (float) x,  0.5f + (float) y, -0.5f + (float) z,  1.0f,  1.0f,  0.0f,  0.6f },
        };
    }

    static std::vector<Face> get_chunk_vertices(std::array<std::array<std::array<BlockFaces, 16>, 16>, 16> chunk_faces) {
        std::vector<Face> vertices{};

        for (int x = 0; x < 16; x++) {
            for (int y = 0; y < 16; y++) {
                for (int z = 0; z < 16; z++) {
                    BlockFaces block_faces = chunk_faces[x][y][z];

                    if (block_faces.block == 1) {
                        if (block_faces.positive_x) {
                            vertices.push_back(get_positive_x_face(x, y, z));
                        }
                        if (block_faces.negative_x) {
                            vertices.push_back(get_negative_x_face(x, y, z));
                        }
                        if (block_faces.positive_y) {
                            vertices.push_back(get_positive_y_face(x, y, z));
                        }
                        if (block_faces.negative_y) {
                            vertices.push_back(get_negative_y_face(x, y, z));
                        }
                        if (block_faces.positive_z) {
                            vertices.push_back(get_positive_z_face(x, y, z));
                        }
                        if (block_faces.negative_z) {
                            vertices.push_back(get_negative_z_face(x, y, z));
                        }
                    }
                }
            }
        }

        return vertices;
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

    static std::vector<float> faces_to_floats(std::vector<Face> faces) {
        std::vector<float> floats;

        for (Face face : faces) {
            floats.push_back(face.vertex0.x);
            floats.push_back(face.vertex0.y);
            floats.push_back(face.vertex0.z);
            floats.push_back(face.vertex0.s);
            floats.push_back(face.vertex0.t);
            floats.push_back(face.vertex0.texture);
            floats.push_back(face.vertex0.brightness);
            floats.push_back(face.vertex1.x);
            floats.push_back(face.vertex1.y);
            floats.push_back(face.vertex1.z);
            floats.push_back(face.vertex1.s);
            floats.push_back(face.vertex1.t);
            floats.push_back(face.vertex1.texture);
            floats.push_back(face.vertex1.brightness);
            floats.push_back(face.vertex2.x);
            floats.push_back(face.vertex2.y);
            floats.push_back(face.vertex2.z);
            floats.push_back(face.vertex2.s);
            floats.push_back(face.vertex2.t);
            floats.push_back(face.vertex2.texture);
            floats.push_back(face.vertex2.brightness);
            floats.push_back(face.vertex3.x);
            floats.push_back(face.vertex3.y);
            floats.push_back(face.vertex3.z);
            floats.push_back(face.vertex3.s);
            floats.push_back(face.vertex3.t);
            floats.push_back(face.vertex3.texture);
            floats.push_back(face.vertex3.brightness);
            floats.push_back(face.vertex4.x);
            floats.push_back(face.vertex4.y);
            floats.push_back(face.vertex4.z);
            floats.push_back(face.vertex4.s);
            floats.push_back(face.vertex4.t);
            floats.push_back(face.vertex4.texture);
            floats.push_back(face.vertex4.brightness);
        }

        return floats;
    }

    static std::vector<float> get_flat_chunk_vertices(int cx, int cz) {
//        std::vector<Face> faces = get_chunk_vertices(get_block_faces(flat_chunk_blocks()));
        return faces_to_floats(get_chunk_vertices(get_block_faces(flat_chunk_blocks(cx, cz))));
    }

    static std::vector<unsigned int> get_flat_chunk_indices(int cx, int cz) {
        return get_indices_of_size(get_flat_chunk_vertices(cx, cz).size());
    }

    static const unsigned int size = 16;

    static std::array<float, 210> cube_positions() {
        std::array<float, 210> positions = {
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.6f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.6f,
             0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.6f,
             0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.6f,
             0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.6f,

             0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.6f,
             0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  0.6f,
            -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.6f,
            -0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.6f,
            -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.6f,

            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.8f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  0.0f,  0.8f,
            -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.8f,
            -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.8f,
            -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  0.0f,  0.8f,

             0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.8f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  0.0f,  0.8f,
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.8f,
             0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.8f,
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  0.0f,  0.8f,

            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f,  1.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,
             0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  1.0f,  1.0f,
             0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f,

             0.5f, -0.5f,  0.5f,  0.0f,  1.0f,  2.0f,  0.4f,
             0.5f, -0.5f, -0.5f,  0.0f,  0.0f,  2.0f,  0.4f,
            -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  2.0f,  0.4f,
            -0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  2.0f,  0.4f,
            -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  2.0f,  0.4f,
        };

        return positions;
    }

    static std::array<unsigned int, 36> cube_indices() {
        std::array<unsigned int, 36> indices = {
                0, 1, 2, 3, 4, 1,
                5, 6, 7, 8, 9, 6,
                10, 11, 12, 13, 14, 11,
                15, 16, 17, 18, 19, 16,
                20, 21, 22, 23, 24, 21,
                25, 26, 27, 28, 29, 26
        };

        return indices;
    }

    static std::array<float, 210> cube_positions_at(float x, float y, float z) {
        std::array<float, 210> positions = cube_positions();

        for (int row = 0; row < 30; row++) {
            positions[row * 7 + 0] += x;
            positions[row * 7 + 1] += y;
            positions[row * 7 + 2] += z;
        }

        return positions;
    }

    static std::array<float, 210 * size * size> flat_chunk_positions() {
        std::array<float, 210 * size * size> positions{};

        for (int x = 0; x < size; x++) {
            for (int z = 0; z < size; z++) {
//                std::array<float, 210> block_positions = cube_positions_at((float) x, roundf(size / 4 * glm::simplex(glm::vec2((float) x / size, (float) z / size))), (float) z);
                std::array<float, 210> block_positions = cube_positions_at((float) x, 0, (float) z);

                for (int i = 0; i < 210; i++) {
                    positions[x * 210 * size + z * 210 + i] = block_positions[i];
                }
            }
        }

        return positions;
    }

    static std::array<float, 210 * size * size> flat_chunk_positions(int chunk_x, int chunk_z) {
        std::array<float, 210 * size * size> positions{};

        for (int x = 0; x < size; x++) {
            for (int z = 0; z < size; z++) {
                std::array<float, 210> block_positions = cube_positions_at((float) x, roundf(size / 4 * glm::perlin(glm::vec2((float) (x + chunk_x * Chunk::size) / size, (float) (z + chunk_z * Chunk::size) / size))), (float) z);

                for (int i = 0; i < 210; i++) {
                    positions[x * 210 * size + z * 210 + i] = block_positions[i];
                }
            }
        }

        return positions;
    }

    static std::array<unsigned int, 36 * size * size> flat_chunk_indices() {
        std::array<unsigned int, 36 * size * size> indices{};

        for (int x = 0; x < size; x++) {
            for (int z = 0; z < size; z++) {
                std::array<unsigned int, 36> block_indices = cube_indices();

                for (int i = 0; i < 36; i++) {
                    indices[x * 36 * size + z * 36 + i] = block_indices[i] + x * 30 * size + z * 30;

                }
            }
        }

        return indices;
    }
};
