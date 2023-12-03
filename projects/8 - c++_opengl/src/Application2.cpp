#include "renderer/Window.h"
#include "renderer/Gui.h"

int main() {
    Window window;
    Gui gui(window.getWindow());

    while (!window.windowShouldClose()) {
        window.pollEvents();

        gui.render();

        window.swapBuffers();
    }

    gui.terminate();
    window.terminate();
    return 0;
}
