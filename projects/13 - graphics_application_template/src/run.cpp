#include "window/window.hpp"
#include "input/input.hpp"
#include "physics/physics.hpp"
#include "matter/matter.hpp"

int main() {
    window::create_context();
    input::create_context();
    physics::create_context();
    matter::create_context();

    while (!window::should_close()) {
        input::poll_events();
        window::update();
        window::render();
    }

    window::terminate();

    return 0;
}
