#include <til.hpp>
#include <algorithm>
#include <cmath>

namespace
{
    constexpr float kMoveStep = 1.0f;
}

int main() {
    til::Framework framework;
    framework.initialize();
    framework.setTargetUpdateRate(60);

    til::Window &window = framework.windowManager.createWindow();
    window.setSize(framework.console.getSize());
    window.setRenderer(&framework.renderer);
    window.depth = 1.0f;

    til::filters::SingleCharacterColored characterFilter('.');
    window.characterPipeline.addFilter(&characterFilter).build();

    til::FilterPipeline<til::filters::VertexData, til::filters::VertexData> fragmentPipeline;
    til::filters::SolidColor solidFill({255, 180, 64, 220});
    fragmentPipeline.addFilter(&solidFill).build();

    const auto initialSize = window.getSize();

    til::primitives::Ellipse ellipse;
    ellipse.center = {0.f, 0.f};
    ellipse.radii = {6.f, 3.5f};

    til::Transform transform;
    transform.setOrigin({0.f, 0.f});

    til::Vector2<til::f32> position = {
        static_cast<til::f32>(initialSize.x) * 0.5f,
        static_cast<til::f32>(initialSize.y) * 0.5f
    };
    float accumulatedTime = 0.f;
    bool running = true;

    while (running) {
        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<til::ConsoleEvent>()) {
                window.setSize(event->newSize);
                continue;
            }

            if (event->isOfType<til::KeyPressEvent>()) {
                switch (event->key) {
                    case til::KeyCode::Escape:
                        running = false;
                        break;
                    case til::KeyCode::Left:
                        position.x -= kMoveStep;
                        break;
                    case til::KeyCode::Right:
                        position.x += kMoveStep;
                        break;
                    case til::KeyCode::Up:
                        position.y -= kMoveStep;
                        break;
                    case til::KeyCode::Down:
                        position.y += kMoveStep;
                        break;
                    default:
                        break;
                }
            }
        }

        const auto size = window.getSize();
        position.x = std::clamp(position.x, 1.f, static_cast<float>(size.x - 2));
        position.y = std::clamp(position.y, 1.f, static_cast<float>(size.y - 2));

        window.fill({10, 12, 16, 255});

        // Draw a coarse grid so movement feedback stays readable.
        for (til::u32 y = 0; y < size.y; ++y) {
            if (y % 5 == 0) {
                for (til::u32 x = 0; x < size.x; ++x) {
                    framework.renderer.drawImmediatePixel(window, {x, y}, {30, 40, 60, 255});
                }
            }
        }

        for (til::u32 x = 0; x < size.x; x += 5) {
            for (til::u32 y = 0; y < size.y; ++y) {
                framework.renderer.drawImmediatePixel(window, {x, y}, {30, 40, 60, 255});
            }
        }

        transform.setPosition(position);

        solidFill.data.color = {
            static_cast<til::u8>(127.0 + 120.0 * std::sin(accumulatedTime)),
            static_cast<til::u8>(127.0 + 120.0 * std::sin(accumulatedTime + 2.09439510239)),
            static_cast<til::u8>(127.0 + 120.0 * std::sin(accumulatedTime + 4.18879020478)),
            220
        };

        framework.renderer.drawImmediate(window, ellipse, transform, fragmentPipeline);

        framework.display();
        framework.update();

        accumulatedTime += til::getDurationInSeconds(framework.getLastUpdateDuration());
    }

    return 0;
}
