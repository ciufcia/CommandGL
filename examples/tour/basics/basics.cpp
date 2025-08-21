#include <cgl.hpp>
#include <iostream>

int main() {
    cgl::Framework framework;
    framework.initialize();

    cgl::filters::CharacterShuffleColored filter;
    filter.data.shufflePeriod = 0.5f;

    framework.characterFilterPipeline.addFilter(&filter).build();

    auto triangle = cgl::Drawable::create<cgl::drawables::Triangle>(
        cgl::Vector2<cgl::f32>(0.f, 0.f),
        cgl::Vector2<cgl::f32>(1.0f, 0.f),
        cgl::Vector2<cgl::f32>(0.5f, 1.0f)
    );

    triangle->transform.setScale({ 20.f, 20.f });

    cgl::filters::UVGradient uvGradientFilter;
    cgl::filters::Grayscale grayscaleFilter;
    cgl::filters::Invert invertFilter;
    triangle->fragmentPipeline.addFilter(&uvGradientFilter).addFilter(&invertFilter).build();

    bool running = true;

    while (running) {
        framework.eventManager.handleEvents(
            [&](cgl::KeyPressEvent, const cgl::Event &event) {
                if (event.key == cgl::KeyCode::Escape) {
                    running = false;
                }
            }
        );

        framework.clearDisplay(cgl::Color{255, 255, 255, 255});

        framework.draw(triangle);

        framework.update();
    }

    return 0;
}