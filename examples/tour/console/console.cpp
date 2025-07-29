#include <cgl.hpp>
#include <iostream>

int main() {
    cgl::Console console;
    console.init();

    auto s = console.getSize();

    std::cout << "Console size: " << s.x << "x" << s.y << std::endl;

    /*
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

        cgl::Console &console = framework.console;

        cgl::Vector2<cgl::u32> size = console.getSize();

        //console.setTitle("Size: " + std::to_string(size.x) + "x" + std::to_string(size.y));

        framework.clearDisplay({255, 255, 255});

        framework.update();
    }
    */
}