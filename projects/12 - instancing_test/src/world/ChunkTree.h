#pragma once

#include <array>

static const int chunk_size = 16;

typedef glm::vec3 position_t;
typedef glm::vec3 color_t;
typedef std::array<color_t, chunk_size * chunk_size * chunk_size> chunk_data_t;

class ChunkTree {
public:
    position_t position;
    color_t color;
    chunk_data_t *chunk_data{};
    typedef std::array<ChunkTree, 8> chunk_trees_t;
    chunk_trees_t *chunk_trees{};

    ChunkTree() : position(), color() {}

    ChunkTree(position_t position_1, color_t color_1) : position(position_1), color(color_1) {}

    void set_chunk_trees(chunk_trees_t *chunk_trees_1) {
        chunk_trees = chunk_trees_1;
    }

    void set_chunk_data(chunk_data_t *chunk_data_1) {
        chunk_data = chunk_data_1;
    }
};
