#include <cgl.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cgl;

int main() {
    float deltaTime = 0.f;
    try {
        Framework framework;

        auto filter = std::make_shared<Filter>();
        filter->type = FilterType::Parallel;
        filter->function = filters::uv;

        auto solidFilter = std::make_shared<Filter>();
        solidFilter->type = FilterType::Parallel;
        solidFilter->function = filters::singleColor;
        solidFilter->data = std::make_shared<filters::SingleColorData>(Color(0, 0, 0, 128));

        auto rectangle = Drawable::create<drawables::Rectangle>(Vector2<f32>(0, 0), Vector2<f32>(40.f, 40.f));
        rectangle->fragmentPipeline.addFilter(solidFilter, 0);
        rectangle->transform.setOrigin({20.f, 20.f});
        rectangle->transform.setPosition({60.f, 60.f});

        auto point = Drawable::create<drawables::Point>(Vector2<f32>{0.f, 0.f});
        point->fragmentPipeline.addFilter(solidFilter, 0);

        auto triangle = Drawable::create<drawables::Triangle>(
            Vector2<f32>{0.f, 0.f},
            Vector2<f32>{40.f, 0.f},
            Vector2<f32>{20.f, 40.f}
        );

        triangle->fragmentPipeline.addFilter(filter, 0);
        triangle->transform.setOrigin({20.0f, 13.333333f});
        triangle->transform.setPosition({80.f, 80.f});
        triangle->depth = 10.f;

        framework.initialize();

        framework.scaleOnBaseSizeDeviation = true;

        while (true) {
            while (auto event = framework.eventManager.pollEvent()) {
                if (event->isOfType<KeyPressEvent>()) {
                    if (event->key == KeyCode::Escape) {
                        throw std::runtime_error("Exit requested by user.");
                    }
                }
            }

            deltaTime = std::chrono::duration_cast<std::chrono::duration<f32>>(framework.clock.tick()).count();

            rectangle->transform.rotate(180.f * deltaTime);
            triangle->transform.rotate(180.f * deltaTime);

            framework.clearDisplay({255, 255, 255, 255});

            framework.draw(rectangle);
            framework.draw(triangle);

            framework.update();
        }
    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    std::cout << deltaTime << " seconds elapsed." << std::endl;

    return 0;
}