#pragma once

#include "ChunkTree.h"

class World {
    World() {
        ChunkTree chunk_tree(position_t(0), color_t(0, 1, 0));
        std::array<ChunkTree, 8> chunk_trees{};
        for (int cx = 0; cx < 2; cx++) {
            for (int cy = 0; cy < 2; cy++) {
                for (int cz = 0; cz < 2; cz++) {
                    ChunkTree chunk(position_t((float) cx, (float) cy, (float) cz), color_t(0, 1, 0));
                    chunk_data_t chunk_data;
                    for (int x = 0; x < chunk_size; x++) {
                        for (int z = 0; z < chunk_size; z++) {
                            chunk_data[x * chunk_size * chunk_size + z] = color_t(0, 1, 0);
                        }
                    }
                    chunk.set_chunk_data(&chunk_data);
                    chunk_trees[cx * 2 * 2 + cy * 2 + cz] = chunk;
                }
            }
        }
        chunk_tree.set_chunk_trees(&chunk_trees);
    }
};
