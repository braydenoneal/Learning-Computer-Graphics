#include <array>

class Chunk {
    std::array<int, 16 * 16 * 16> data{0};
    glm::vec3 position;

    Chunk(glm::vec3 chunk_position) : position(chunk_position) {
        for (int x = 0; x < 16; x++) {
            for (int z = 0; z < 16; z++) {
                float scale = 128;
                float noise = glm::perlin(glm::vec2(((float) x + 16 * position.x) / scale, ((float) z + 16 * position.z) / scale));
                int y_cutoff = std::floor(noise * 32);
                if (y_cutoff >= 0 && y_cutoff < 16) {
                    for (int y = 0; y <= y_cutoff; y++) {
                        data[x * 16 * 16 + y * 16 + z] = 1;
                    }
                }
            }
        }
    }


};
