#include <cgl.hpp>

int main() {
    cgl::Framework framework;

    framework.initialize();

    bool running = true;

    while (running) {
        framework.eventManager.handleEvents(
            [&](cgl::KeyPressEvent, const cgl::Event &event) {
                if (event.key == cgl::KeyCode::Escape) {
                    running = false;
                }
            }
        );

        framework.clearDisplay({255, 255, 255});

        framework.update();
    }
}