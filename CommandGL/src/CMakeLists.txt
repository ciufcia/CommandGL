add_library(
    ${PROJECT_NAME}
    cgl.cpp
    console.cpp
    event.cpp
    event_manager.cpp
    global_memory.cpp
    color.cpp
    framework.cpp
    filters.cpp
    screen_buffer.cpp
    character_buffer.cpp
    transform.cpp
    drawable.cpp
    timing.cpp
    texture.cpp
    text.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

message(${CMAKE_CURRENT_SOURCE_DIR} "Current source directory")

target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)