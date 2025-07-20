<div align="center">

# CommandGL

## Command line graphics library for Windows written in C++.

### [About ❔](#about)
### [Building ⚒️](#building)
### [Tour 🗺️](examples/tour/table_of_contents.md)
### [Showcase](#showcase) ⭐
### [Documentation 📘](https://ciufcia.github.io/CommandGL/)

</div>

## About

CommandGL (cgl) is a modern C++ 20 framework for high-performance rasterization and interactive graphics directly in the Windows console. It provides efficient rendering, robust input handling, and a flexible architecture designed for both customization and ease of use. CommandGL empowers developers to create advanced console-based graphical applications with minimal overhead and maximum control.

## Building

### Prerequisites:

- **Windows** as the operating system
- **CMake** (version 3.26 or higher)
- **C++ 20 compatible compiler**
- **OpenMP**

### Option 1 - use FetchContent

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
### Option 2 - build manually

#### 1. Clone the repository.

```sh
git clone https://github.com/ciufcia/CommandGL.git && cd CommandGL
```

#### 2. Create a build directory.

```sh
mkdir build && cd build
```

#### 3. Generate project files using CMake.

```sh
cmake ..
```

#### 4. Build the project using CMake.

```sh
cmake --build .
```

#### 5. Install.

```sh
cmake --install .
```

#### 6. You can now link to CommandGL via:

```cmake
find_package(CommandGL REQUIRED)

target_link_libraries(your_target PRIVATE CommandGL::CommandGL)
```

## Showcase

nothing here :(