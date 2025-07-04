#include <cgl.hpp>
#include <iostream>
#include <vector>
#include <string>

using namespace cgl;

int main() {
    float deltaTime = 0.f;
    try {
        Framework framework;

        auto texture = Texture::create("img.jpg");

        auto filter = Filter::create(
            FilterType::Parallel,
            filters::texture,
            filters::TextureData{texture, Texture::SamplingMode::Bilinear}
        );

        auto ellipse = Drawable::create<drawables::Ellipse>(Vector2<f32>(0, 0), Vector2<f32>(40.f, 20.f));
        ellipse->fragmentPipeline.addFilter(filter, 0);
        ellipse->transform.setPosition({40.f, 40.f});

        framework.initialize();

        framework.console.setTitle("CommandGL Example");

        framework.scaleOnBaseSizeDeviation = true;

        while (true) {
            while (auto event = framework.eventManager.pollEvent()) {
                if (event->isOfType<KeyPressEvent>()) {
                    if (event->key == KeyCode::Escape) {
                        throw std::runtime_error("Exit requested by user.");
                    }
                }
            }

            deltaTime = getDurationInSeconds(framework.clock.tick());

            framework.clearDisplay({255, 255, 255, 255});

            framework.draw(ellipse);

            framework.update();
        }

    } catch (const std::exception& ex) {
        std::cerr << "Exception: " << ex.what() << std::endl;
    } catch (...) {
        std::cerr << "Unknown exception occurred." << std::endl;
    }

    return 0;
}