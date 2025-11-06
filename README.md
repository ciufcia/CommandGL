# Textil

Textil is a modern C++20 framework for building rich, animated graphics directly in the terminal. It wraps console configuration, rendering, window management, timing, and input into a single, cohesive API that works the same on Windows, Linux, and macOS.

## Highlights
- Cross-platform console backends built on WinAPI, termios/libevdev, and macOS HID
- Immediate-mode renderer with primitives, meshes, textures, and blend modes
- Filter pipelines for post-processing and character conversion
- Window manager for layered composition and z-ordered layout
- Event system covering keyboard, mouse, and console notifications
- MIT licensed and ready to embed in existing CMake projects

## Platform Support and Dependencies
- Windows: Visual Studio 2022 or newer with OpenMP support enabled
- Linux: GCC or Clang with OpenMP; requires `libevdev` headers for raw input
- macOS: Clang with OpenMP (`brew install libomp`), HID permissions enabled
- All platforms: CMake 3.26+, a compiler with full C++20 support, and a terminal capable of 24-bit color escape codes

## Quick Start

### FetchContent
```cmake
include(FetchContent)

FetchContent_Declare(
    Textil
    GIT_REPOSITORY https://github.com/ciufcia/Textil.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(Textil)

target_link_libraries(your_target PRIVATE Textil)
```

### Build from Source
```sh
git clone https://github.com/ciufcia/Textil.git
cd Textil
cmake -S . -B build
cmake --build build
cmake --install build --prefix "${PWD}/dist"
```

### Minimal Program
```cpp
#include <til.hpp>

int main() {
    til::Framework framework;
    framework.initialize();
    framework.setTargetUpdateRate(30);

    auto &window = framework.windowManager.createWindow();
    window.setSize({64, 24});
    window.setRenderer(&framework.renderer);

    til::filters::SingleCharacterColored glyph('#');
    window.characterPipeline.addFilter(&glyph).build();

    bool running = true;
    while (running) {
        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<til::KeyPressEvent>() && event->key == til::KeyCode::Escape) {
                running = false;
            }
        }

        window.fill({20, 20, 40, 255});
        framework.renderer.drawImmediatePixel(window, {10, 10}, {255, 120, 0, 255});

        framework.display();
        framework.update();
    }

    return 0;
}
```

## Examples
- `examples/minimal_loop`: color sweep and event loop boilerplate (`cmake --build build --target minimal_loop`)
- `examples/interactive_canvas`: arrow-key controlled ellipse demonstrating filter pipelines (`cmake --build build --target interactive_canvas`)
- `examples/basics`: rotating mesh with character and fragment filters (`cmake --build build --target basics`)
- `examples/raycast_demo`: pseudo-3D raycaster with WASD + QE/arrow rotation controls (`cmake --build build --target raycast_demo`)

See `docs/examples.md` for usage notes and expected behaviour.

## Documentation
- Header-based API reference is generated with Doxygen: `doxygen docs/Doxyfile`
- `docs/getting_started.md` covers architecture, frame flow, and integration tips
- Prebuilt documentation is published at https://ciufcia.github.io/Textil/

## License
Textil is released under the MIT License. See `LICENSE` for details.

