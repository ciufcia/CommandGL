#include <til.hpp>
#include <algorithm>
#include <cmath>

int main() {
    til::Framework framework;
    framework.initialize();
    framework.setTargetUpdateRate(30);

    til::Window &window = framework.windowManager.createWindow();
    window.setSize(framework.console.getSize());
    window.setRenderer(&framework.renderer);

    til::filters::SingleCharacterColored characterFilter('@');
    window.characterPipeline.addFilter(&characterFilter).build();

    bool running = true;
    float phase = 0.f;

    while (running) {
        bool quitNow = false;
        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<til::ConsoleEvent>()) {
                window.setSize(event->newSize);
                continue;
            }

            if (event->isOfType<til::KeyPressEvent>() && event->key == til::KeyCode::Escape) {
                quitNow = true;
                break;
            }
        }
        if (quitNow) {
            break;
        }

        window.fill({12, 12, 18, 255});

        const auto size = window.getSize();
        for (til::u32 x = 0; x < size.x; ++x) {
            const float normalized = static_cast<float>(x) / static_cast<float>(size.x);
            const double angle = normalized * 6.28318530718 + phase;

            const til::u8 r = static_cast<til::u8>(127.0 + 120.0 * std::sin(angle));
            const til::u8 g = static_cast<til::u8>(127.0 + 120.0 * std::sin(angle + 2.09439510239));
            const til::u8 b = static_cast<til::u8>(127.0 + 120.0 * std::sin(angle + 4.18879020478));

            const double wave = std::sin(phase * 0.5 + normalized * 3.1415926535);
            const float yFloat = static_cast<float>((wave * 0.5 + 0.5) * static_cast<double>(size.y - 1));
            const til::u32 y = static_cast<til::u32>(std::clamp(yFloat, 0.0f, static_cast<float>(size.y - 1)));

            framework.renderer.drawImmediatePixel(window, {x, y}, {r, g, b, 255});
        }

        framework.display();
        framework.update();
        phase += 0.07f;
    }

    return 0;
}
