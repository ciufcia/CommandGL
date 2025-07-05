<div align="center">

# 1. Setup

## [Back to Table of Contents](../table_of_contents.md)

</div>

This chapter will teach you how to create a simple cgl app.

## 1. Including the library

Once you've built the library and linked it to your project, you can just:
```cpp
#include <cgl.hpp>
```

## 2. Initializing the framework
To use the framework, you need to initialize it first:
```cpp
...

int main() {
    cgl::Framework framework;

    framework.initialize();
}
```

## 3. A basic app loop
Paste this code after `framework.initialize();`.
Don't worry if you don't understand something. Everything here will be explained in the next chapters.
```cpp
...

bool running = true;

while (running) {
    framework.eventManager.handleEvents(
        [&](cgl::KeyPressEvent, const cgl::Event &event) {
            if (event.key == cgl::KeyCode::Escape) {
                running = false;
            }
        }
    );

    framework.clearDisplay({255, 255, 255});

    framework.update();
}

...
```
Two note-worthy functions here are `clearDisplay()` which fills the display with a chosen RGB color (white in this case) and `update()`. The update function must be called every frame, ideally at the end of the frame for the library to function correctly.

After running this you should see a console filled with white '@'. When pressing Escape the program should quit.

<div align="center">

# [Next Chapter: Console](../console/console.md) | [Go to the Top](#1-setup) | [Code from this Chapter](setup.cpp)

</div>