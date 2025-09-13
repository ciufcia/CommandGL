#include <til.hpp>

int main() {
    til::Framework framework;

    framework.initialize();

    framework.setTargetUpdateRate(10);

    til::Window &defaultWindow = framework.windowManager.createWindow();
    defaultWindow.setSize({30, 30});
    defaultWindow.setRenderer(&framework.renderer);

    til::Window &secondWindow = framework.windowManager.createWindow();
    secondWindow.setSize({40, 40});
    secondWindow.setPosition({50, 10});
    secondWindow.setRenderer(&framework.renderer);

    til::filters::SingleColoredDithered singleColoredDitheredFilter({255, 255, 255, 255});
    defaultWindow.characterPipeline.addFilter(&singleColoredDitheredFilter).build();

    til::filters::SingleCharacterColored singleCharFilter(64);
    secondWindow.characterPipeline.addFilter(&singleCharFilter).build();

    til::Transform transform;

    til::FilterPipeline<til::filters::VertexData, til::filters::VertexData> filterPipeline;
    til::filters::SolidColor solidColorFilter({255, 0, 0, 64});
    filterPipeline.addFilter(&solidColorFilter).build();

    til::primitives::Vertex triangleVertices[6] = {
        // First triangle (clockwise): bottom-left -> top-left -> top-right
        {{5.f, 15.f}, {0.f, 1.f}},   // bottom-left
        {{5.f, 5.f}, {0.f, 0.f}},    // top-left
        {{15.f, 5.f}, {1.f, 0.f}},   // top-right
        
        // Second triangle (clockwise): bottom-left -> top-right -> bottom-right  
        {{5.f, 15.f}, {0.f, 1.f}},   // bottom-left
        {{15.f, 5.f}, {1.f, 0.f}},   // top-right
        {{15.f, 15.f}, {1.f, 1.f}}   // bottom-right
    };

    til::u32 meshId = framework.renderer.addMesh(triangleVertices, 6);
    
    til::primitives::TriangleMesh mesh;
    mesh.vertexCount = 6;

    while (true) {
        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<til::KeyPressEvent>()) {
                if (event->key == til::KeyCode::Escape) {
                    return 0;
                }
            }
        }

        transform.rotate(0.05f);

        til::u32 meshId = framework.renderer.addMesh(triangleVertices, mesh.vertexCount);
        mesh.firstVertex = meshId;

        defaultWindow.fill({255, 255, 255});
        secondWindow.fill({0, 0, 255});

        framework.renderer.drawImmediate(defaultWindow, mesh, transform, filterPipeline);

        framework.display();
        framework.update();
    }

    return 0;
}