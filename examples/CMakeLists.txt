cmake_minimum_required(VERSION 3.16)
project(cgl_example)

set(CMAKE_CXX_STANDARD 20)

add_executable(test test.cpp)

add_subdirectory(../CommandGL ${CMAKE_BINARY_DIR}/cgl_build)

target_link_libraries(test PRIVATE CommandGL)

target_include_directories(test PRIVATE ../CommandGL/include)
