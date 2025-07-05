#include <cgl.hpp>

int main() {
    cgl::Framework framework;

    framework.initialize();

    bool running = true;

    cgl::Color color = {255, 255, 255};

    while (running) {
        framework.eventManager.handleEvents(
            [&](cgl::KeyPressEvent, const cgl::Event &event) {
                if (event.key == cgl::KeyCode::Escape) {
                    running = false;
                }
            }
        );

        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<cgl::ConsoleEvent>()) {
                if (color == cgl::Color(255, 255, 255)) {
                    color = {0, 0, 0}; // Switch to black
                } else {
                    color = {255, 255, 255}; // Switch to white
                }
            }
        }

        framework.clearDisplay(color);

        framework.update();
    }
}