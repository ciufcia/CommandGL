cmake_minimum_required(VERSION 3.26)

project(
    CommandGL
    VERSION 1.0.0
    DESCRIPTION "Command line graphics library for Windows written in C++."
    LANGUAGES CXX
)

option(BUILD_EXAMPLES "Build examples" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(BUILD_SHARED_LIBS OFF)
set(WINDOWS_EXPORT_ALL_SYMBOLS TRUE)

# Find OpenMP for parallel processing
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
else()
    message(FATAL_ERROR "OpenMP not found. Please install OpenMP support for your compiler.")
endif()

add_subdirectory(CommandGL)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()