#include <til.hpp>
#include <iostream>

int main() {
    til::Framework framework;

    framework.initialize();

    til::Window &defaultWindow = framework.windowManager.createWindow();
    defaultWindow.setSize({30, 30});
    defaultWindow.setRenderer(&framework.renderer);

    til::Window &secondWindow = framework.windowManager.createWindow();
    secondWindow.setSize({40, 40});
    secondWindow.setPosition({50, 10});
    secondWindow.setRenderer(&framework.renderer);

    til::filters::CharacterShuffleColored charShuffleFilter;
    defaultWindow.characterPipeline.addFilter(&charShuffleFilter).build();

    til::filters::SingleCharacterColored singleCharFilter(64);
    secondWindow.characterPipeline.addFilter(&singleCharFilter).build();

    til::Transform transform;

    til::FilterPipeline<til::filters::VertexData, til::filters::VertexData> filterPipeline;
    til::filters::SolidColor solidColorFilter({255, 0, 0, 255});
    filterPipeline.addFilter(&solidColorFilter).build();

    til::primitives::Ellipse ellipse;
    ellipse.center = {15.f, 15.f};
    ellipse.radii = {5.f, 5.f};

    while (true) {
        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<til::KeyPressEvent>()) {
                if (event->key == til::KeyCode::Escape) {
                    return 0;
                }
            }
        }

        transform.rotate(0.05f);

        defaultWindow.fill({255, 255, 255});
        secondWindow.fill({0, 0, 255});

        framework.renderer.drawImmediate(defaultWindow, ellipse, transform, filterPipeline);

        framework.display();
        framework.update();
    }

    return 0;
}