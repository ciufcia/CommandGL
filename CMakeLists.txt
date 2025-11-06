cmake_minimum_required(VERSION 3.26)

project(
    Textil
    VERSION 1.0.0
    DESCRIPTION "Cross-platform terminal graphics library for modern C++."
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

if(APPLE)
    set(LIBOMP_PATH "/usr/local/opt/libomp/lib/libomp.dylib" CACHE FILEPATH "Path to libomp library")
    set(OpenMP_CXX_FLAGS "-Xpreprocessor -fopenmp")
    set(OpenMP_CXX_LIB_NAMES "omp")
    set(OpenMP_omp_LIBRARY "${LIBOMP_PATH}")
endif()

find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_VERSION}")
else()
    message(FATAL_ERROR "OpenMP not found. Please install OpenMP support for your compiler.")
endif()

add_subdirectory(Textil)
if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()