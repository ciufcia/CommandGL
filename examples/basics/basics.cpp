#include <til.hpp>
#include <iostream>

int main() {


    /* Framework Initialization */

    // Initialize the framework
    til::Framework framework;
    framework.initialize();


    /* Set the way in which pixels will be represented in the console */

    // You can use other filters provided by the library or create your own.
    // As reference you can see how pre-implemented filters work
    til::filters::SingleColoredDithered characterFilter { til::Color(255, 255, 255) };
    framework.characterFilterPipeline.addFilter(&characterFilter).build();


    /* Creating a drawable */

    // Create a drawable triangle
    std::shared_ptr<til::drawables::Triangle> triangle = til::Drawable::create<til::drawables::Triangle>(
        til::Vector2<til::f32>(0.f, 0.f),
        til::Vector2<til::f32>(1.0f, 0.f),
        til::Vector2<til::f32>(0.5f, 1.0f)
    );


    /* Modifying a drawable's Transform */

    // Calculate the centroid of the triangle
    til::Vector2<til::f32> centre;
    for (auto &point : triangle->points) {
        centre += point;
    }
    centre /= 3;

    // Set the triangle's transformation origin to its centroid
    triangle->transform.setOrigin(centre);

    // Set the triangle's scale
    triangle->transform.setScale({ 20.f, 20.f });

    // Set the triangle's position to the middle of the screen
    triangle->transform.setPosition(framework.console.getSize() / 2.f);

    /* Modify the way the drawable is displayed */

    // Let's display a gradient on the triangle and then invert the color of each pixel
    til::filters::UVGradient uvGradientFilter;
    til::filters::Invert invertFilter;
    triangle->fragmentPipeline.addFilter(&uvGradientFilter).addFilter(&invertFilter).build();


    /* Run the app */

    // Set the target frames per second
    framework.setFPSTarget(60);

    // Flag that indicates whether the application is running
    bool running = true;

    // Main app loop
    while (running) {
        // Handle framework events via callbacks
        framework.eventManager.handleEvents(
            // Handle key presses
            [&](til::KeyPressEvent, const til::Event &event) {
                // stop the app if Escape was pressed
                if (event.key == til::KeyCode::Escape) {
                    running = false;
                }
            },
            // Handle mouse movement
            [&](til::MouseMoveEvent, const til::Event &event) {
                // Make the triangle move with the mouse
                til::f32 sensitivity = 0.1f;
                triangle->transform.move(static_cast<til::Vector2<til::f32>>(event.mouseDelta) * sensitivity);
            }
        );

        // Rotate the triangle taking deltaTime into consideration
        triangle->transform.rotate(180.f * til::getDurationInSeconds(framework.getLastFrameTime()));

        // Make the screen white
        framework.clearDisplay(til::Color{0, 0, 0, 255});

        // Draw the drawable
        framework.draw(triangle);

        // Conclude application frame
        framework.update();
    }

    return 0;
}