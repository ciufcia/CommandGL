<div align="center">

# 3. Events

## [Back to Table of Contents](../table_of_contents.md)

</div>

This chapter will teach you how to handle user input, as well as react to console size changes.
In this chapter we will be modifying the code from the first chapter, which can be found [here](../setup/basic_app.cpp).

## 1. Event manager

In CommandGL all user input handling is done via the `eventManager` member of the `Framework` class:

```cpp
...

cgl::EventManager &eventManager = framework.eventManager;

...
```

## 2. Checking key states

To check whether a specific key is pressed, you can use the `getKeyState()` method of the event manager:

```cpp
if (eventManager.isKeyPressed(cgl::KeyCode::A)) {
    // do something if 'A' is pressed
}
```

You can also query the states of all keys, like so:

```cpp
auto keyStates = eventManager.getKeyStates();

if (keyStates[(int)cgl::KeyCode::A]) {
    // do something if 'A' was pressed
}
```

## 3. Handling events

But what if you wanted to get "alerted" instead of having to check of presses/changes. That's what events are for! CommandGL supports two ways in which you can handle them:

### 1. Callbacks

You can pass callbacks to `eventManager.handleEvents()`. The Library will automatically match the function signatures to relevant events and call the functions accordingly when a compatible event occurs:

```cpp
...

framework.eventManager.handleEvents(
    // this function will be called for each KeyPressEvent
    [&](cgl::KeyPressEvent, const cgl::Event &event) {
        // check the key that was pressed
        if (event.key == cgl::KeyCode::Escape) {
            running = false;
        }
    }
);

...
```

### 2. Polling

You can also handle events by polling them in a loop, one by one:

```cpp
...

while (auto event = framework.eventManager.pollEvent()) {
    if (event->isOfType<cgl::ConsoleEvent>()) {
        // do something on window resize
    }
}

...
```

### You can also combine the two approaches

```cpp
...

framework.eventManager.handleEvents(
    // this function will be called for each KeyPressEvent
    [&](cgl::KeyPressEvent, const cgl::Event &event) {
        // check the key that was pressed
        if (event.key == cgl::KeyCode::Escape) {
            running = false;
        }
    }
);

// all the events not processed by handleEvents (everything except for key presses) will be polled here.
while (auto event = framework.eventManager.pollEvent()) {
    if (event->isOfType<cgl::ConsoleEvent>()) {
        // do something on window resize
    }
}

...
```

## 4. Example usage

Let's now make it so that the screen switches between white and black, when resized:

```cpp
    ...

    cgl::Color color = {255, 255, 255};

    while (running) {
        framework.eventManager.handleEvents(
            [&](cgl::KeyPressEvent, const cgl::Event &event) {
                if (event.key == cgl::KeyCode::Escape) {
                    running = false;
                }
            }
        );

        while (auto event = framework.eventManager.pollEvent()) {
            if (event->isOfType<cgl::ConsoleEvent>()) {
                if (color == cgl::Color(255, 255, 255)) {
                    color = {0, 0, 0}; // Switch to black
                } else {
                    color = {255, 255, 255}; // Switch to white
                }
            }
        }

        framework.clearDisplay(color);

        ....
```

<div align="center">

# [Next Chapter: Drawing](../drawing/drawing.md) | [Go to the Top](#3-events) | [Code from this Chapter](events.cpp)

</div>