#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

#include "renderer/Window.h"
#include "renderer/Gui.h"
#include "renderer/Renderer.h"
#include "renderer/Cube.h"
#include "renderer/Transform.h"
#include "renderer/Keymap.h"
#include "renderer/Context.h"

int main() {
    Window window;

    Keymap keymap;
//    window.set_callback(keymap.callback);

    Gui gui(window.getWindow());

    {
        glm::mat4 pers = glm::perspective(glm::radians(70.0f), window.get_window_aspect_ratio(), 0.1f, 100.0f);

        auto camera_position = glm::vec3(0.0f, 0.0f, -5.5f);
        auto camera_angle = glm::vec3(0.0f);
        auto model_position = glm::vec3(0.0f);
        auto model_angle = glm::vec3(0.0f);

        Context context = { camera_position, camera_angle, model_position, model_angle };

        Renderer renderer;

        while (!window.window_should_close()) {
            window.poll_events();

            renderer.clear();

            if (model_angle.x > 3.14f) {
                model_angle -= 3.14f * 2;
            }

            model_angle += 0.01f;

            Cube::draw(renderer, pers * render_transformation(camera_position, camera_angle, model_position, model_angle));

            gui.render();

            window.swap_buffers();
        }
    }

    gui.terminate();
    window.terminate();

    return 0;
}
