# Getting Started with Textil

Textil targets developers who want predictable, high-level access to terminal rendering without giving up control. This guide summarises the moving pieces so you can embed the library quickly and confidently.

## Build Requirements
- CMake 3.26 or newer
- A C++20 compiler with OpenMP enabled (MSVC 19.36+, Clang 15+, GCC 13+)
- Terminal with 24-bit colour and ANSI escape support
- Platform specifics:
  - **Windows:** no extra dependencies beyond the compiler toolset
  - **Linux:** install `libevdev` development headers for raw keyboard and mouse input
  - **macOS:** install libomp via Homebrew (`brew install libomp`) and allow HID access

## Library Layout
- `til::Framework` orchestrates the subsystems and owns `Console`, `Renderer`, `WindowManager`, and `EventManager`
- `Console` abstracts platform-specific terminal configuration, buffer flush, and device discovery
- `Renderer` provides immediate-mode drawing helpers, mesh batching, filter pipelines, and blending
- `Window` and `WindowManager` create layered render targets, each with its own post-process and character pipelines
- `EventManager` exposes input as a FIFO queue with polling or callback dispatch patterns
- `filters`, `textures`, `drawables`, and `timing` add specialised utilities for effects, asset handling, and frame control

## Frame Lifecycle
1. `Framework::initialize()` configures the console, clears state, and primes timing
2. Each loop iteration should:
   - Call `Framework::update()` to ingest events, clear transient renderer state, and update timing data
   - Consume events through `EventManager::pollEvent()` or `handleEvents()`
   - Submit draw calls to windows (fill buffers, draw primitives, run custom filters)
   - Call `Framework::display()` to process window pipelines, composite, and flush to the terminal
3. Optionally limit speed with `Framework::setTargetUpdateRate()` or `setTargetUpdateDuration()`

## Integrating Textil into a Project
1. Add the library via CMake (`FetchContent` or `find_package(Textil REQUIRED)` after installation)
2. Include `til.hpp` in translation units that require the framework
3. Create a `Window`, set its size, position, depth, and assign the shared renderer via `window.setRenderer(&framework.renderer)`
4. Configure the character pipeline with filters such as `filters::SingleCharacterColored` or `filters::SingleColoredDithered`
5. Build fragment pipelines for complex primitives using `FilterPipeline<filters::VertexData, filters::VertexData>`
6. Use `Renderer::drawImmediatePixel`, `drawImmediate` (with vertices, lines, ellipses, or meshes), or `addMesh`/`drawMesh` flows for retained-mode scenarios

## Working with Filters
- Pipelines are lazy; call `build()` after adding filters to validate types and allocate intermediate buffers
- Filter instances live outside the pipeline and must outlive it, enabling runtime changes via their public `data` fields
- Text filters convert colour buffers into `CharacterCell` buffers which the console ultimately prints
- Combine post-process filters (e.g. `UVGradient`, `Grayscale`, `Invert`) with character filters to achieve stylised effects

## Timing and Animation
- `Framework::getLastUpdateDuration()` returns the time spent on the most recent update call
- Use helpers in `til::timing` such as `getDurationInSeconds()` to convert to floating-point seconds
- Attach real time to filter pipelines by reading and writing `window.getBaseData().time`

## Error Handling
- Textil throws `til::LogicError`, `til::InvalidArgumentError`, and platform-specific errors when invariants are violated
- All errors inherit from `std::runtime_error`, making it easy to surface failure from your main loop

Refer to `docs/examples.md` and the new sample programs under `examples/` for practical walkthroughs once you are comfortable with these concepts.
