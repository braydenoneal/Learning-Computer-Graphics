#include "Context.h"
#include "../src2/ChunkData.h"
#include "../src2/DrawChunk.h"
#include <glm/gtc/noise.hpp>

int main() {
    WindowContext window_context;
    GameContext game_context;
    Input input(&window_context, &game_context);
    GuiContext gui_context(&window_context, &game_context);
    Renderer renderer(&window_context, &game_context);

    // create chunk data
    auto chunks = ChunkData::get_flat_chunks(game_context.cube_count);
    // create chunk meshes
    auto meshes = ChunkData::get_chunk_meshes(chunks);
    // create shared layout, shader, and texture
    DrawChunk draw_chunk;

    while(!window_context.window_should_close()) {
        WindowContext::handle_events();
        input.do_actions();
        renderer.render();
        // draw each chunk mesh on the shared draw setup
        glm::mat4 perspective = Transform::perspective_transformation(window_context.field_of_view, window_context.get_aspect_ratio(), 0.01f, 10000.0f);
        UniformsPackage uniforms_package = { perspective, game_context.camera_position, game_context.camera_angle };
        draw_chunk.draw_all(uniforms_package, meshes, game_context.cube_count);
        gui_context.render();
        window_context.swap_buffers();
    }

    GuiContext::terminate();
    WindowContext::terminate();

    return 0;
}

/*
 * Update {
 *      Get input
 *      Change window settings, matter, and laws if necessary
 * }
 *
 * Render {
 *      Render from window settings, matter, and laws
 * }
 *
 * Input:
 *
 */
