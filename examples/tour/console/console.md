<div align="center">

# 2. Console

## [Back to Table of Contents](../table_of_contents.md)

</div>

This chapter will teach you how fetch data or modify the console in which your app is running.
In this chapter we will be modifying the code from the previous chapter, which can be found [here](../setup/basic_app.cpp).

## 1. Accessing the console

Console related functionality can be accessed via the `console` member of the `Framework` class:

```cpp
...

cgl::Console &console = framework.console;

...
```

## 2. Fetching some data

With the reference to the console, we can extract things such as the size of the console (in characters), font used by the console, as well as the title of the console window.

```cpp
cgl::Vector2<u32> consoleSize  = console.getSize();
cgl::ConsoleFont  consoleFont  = console.getFont();
std::string       consoleTitle = console.getTitle();
```

The only non-trivial type here is `ConsoleFont`, about which more can be read [here](https://ciufcia.github.io/CommandGL/classcgl_1_1ConsoleFont.html).

## 3. Modifying the console

Unfortunately different console hosts on windows may support only some of the console modifications. The Legacy Console Host should support all modifications, meanwhile the new Windows Terminal only supports changing the title. Calling Unsupported methods of console will result in undefined behaviour. If you wished to, you can modify the console with these commands:

```cpp
console.setSize(consoleSize);
console.setFont(consoleFont);
console.setTitle(consoleTitle);
```

## 4. Tracking the window size

Now let's make our app report the window size to the user via the window title:

```cpp
...

cgl::Vector2<u32> size = console.getSize();

console.setTitle("Size: " + std::to_string(size.x) + "x" + std::to_string(size.y));
...
```

When resizing the console window, you should now be able to see the title updating with the correct size.

<div align="center">

# [Next Chapter: Events](../events/events.md) | [Go to the Top](#2-console) | [Code from this Chapter](console.cpp)

</div>