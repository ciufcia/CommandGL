# Textil Examples

The repository ships with small programs that illustrate different subsystems. Build them by enabling `BUILD_EXAMPLES` (ON by default) and compiling the desired target from your build directory.

```sh
cmake -S . -B build
cmake --build build --target <example-target>
```

| Target | Location | Demonstrates | Notes |
| ------ | -------- | ------------ | ----- |
| `minimal_loop` | `examples/minimal_loop` | Framework boilerplate, event polling, per-frame drawing | Displays a colour sweep and exits on `Escape` |
| `interactive_canvas` | `examples/interactive_canvas` | Filter pipelines, transforms, keyboard control | Move the glowing ellipse with arrow keys, `Escape` to quit |
| `basics` | `examples/basics` | Mesh rendering, rotating transforms, character dithering | Spins a triangle mesh across two windows |

All examples expect a terminal that supports ANSI escape sequences and full RGB colour. When running on Linux you may need to grant the binary permission to read `/dev/input/event*` devices (typically by running from a seat with access to the input group).
