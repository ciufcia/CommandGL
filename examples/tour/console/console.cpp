#include <cgl.hpp>
#include <iostream>

int main() {
    cgl::Console console;
    console.init();

    auto s = console.getSize();

    std::vector<cgl::Event> events;

    while (true) {
        events.clear();
        console.getEvents(events);
        for (const auto &event : events) {
            if (event.isOfType<cgl::KeyPressEvent>()) {
                std::cout << "Key Pressed: " << static_cast<int>(event.key) << std::endl;
            } else if (event.isOfType<cgl::KeyReleaseEvent>()) {
                std::cout << "Key Released: " << static_cast<int>(event.key) << std::endl;
            } else if (event.isOfType<cgl::MouseMoveEvent>()) {
                std::cout << "Mouse Moved By: (" << event.mouseDelta.x << ", " << event.mouseDelta.y << ")" << std::endl;
            } else if (event.isOfType<cgl::MouseScrollEvent>()) {
                std::cout << "Mouse Scrolled: " << static_cast<int>(event.mouseScrollDelta) << std::endl;
            } else if (event.isOfType<cgl::ConsoleEvent>()) {
                std::cout << "Console Resized to: (" << event.newSize.x << ", " << event.newSize.y << ")" << std::endl;
            }
        }
    }

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