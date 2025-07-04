<div align="center">

# CommandGL

## Command line graphics library for Windows written in C++.

### [About ❔](#about)
### [Building ⚒️](#building)
### Tour 🗺️
### [Showcase](#showcase) ⭐
### [Documentation 📘](https://ciufcia.github.io/CommandGL/)

</div>

## About

<div style="margin-left: 2em">

CommandGL (cgl) is a modern C++ 20 framework for high-performance rasterization and interactive graphics directly in the Windows console. It provides efficient rendering, robust input handling, and a flexible architecture designed for both customization and ease of use. CommandGL empowers developers to create advanced console-based graphical applications with minimal overhead and maximum control.

</div>

## Building

<div style="margin-left: 2em">

### Prerequisites:

- **Windows** as the operating system
- **CMake** (version 3.26 or higher)
- **C++ 20 compatible compiler**
- **OpenMP**

### Option 1 - use FetchContent

<div style="margin-left: 4em">

You can add CommandGL as a dependency to your own CMake project using [FetchContent](https://cmake.org/cmake/help/latest/module/FetchContent.html):

```cmake
include(FetchContent)

FetchContent_Declare(
    CommandGL
    GIT_REPOSITORY https://github.com/ciufcia/CommandGL.git
    GIT_TAG        main
)

FetchContent_MakeAvailable(CommandGL)

target_link_libraries(your_target PRIVATE CommandGL)
```

</div>

### Option 2 - build manually

<div style="margin-left: 2em">

#### 1. Clone the repository.

<div style="margin-left: 2em">

```sh
git clone https://github.com/ciufcia/CommandGL.git && cd CommandGL
```

</div>

#### 2. Create a build directory.

<div style="margin-left: 2em">

```sh
mkdir build && cd build
```

</div>

#### 3. Generate project files using CMake.

<div style="margin-left: 2em">

```sh
cmake ..
```

</div>

#### 4. Build the project using CMake.

<div style="margin-left: 2em">

```sh
cmake --build .
```

</div>

</div>

</div>

## Showcase

nothing here :(