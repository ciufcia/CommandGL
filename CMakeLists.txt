cmake_minimum_required(VERSION 3.26)

project(
    CommandGL
    VERSION 1.0.0
    DESCRIPTION "Command line graphics library for Windows written in C++."
    LANGUAGES CXX
)

if(NOT WIN32)
    message(FATAL_ERROR "CommandGL only works on windows")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

# Find OpenMP for parallel processing
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
else()
    message(FATAL_ERROR "OpenMP not found. Please install OpenMP support for your compiler.")
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

option(BUILD_SHARED_LIBS OFF)

include(FetchContent)

FetchContent_Declare(
    utf8cpp
    GIT_REPOSITORY https://github.com/nemtrif/utfcpp.git
    GIT_TAG        v4.0.6
)

FetchContent_MakeAvailable(utf8cpp)

add_subdirectory(CommandGL)
add_subdirectory(examples)
