add_library(
    ${PROJECT_NAME}
    cgl.cpp
    keycodes.cpp
    console.cpp
    vector2.cpp
    filters.cpp
    global_memory.cpp
    color.cpp
    framework.cpp
    transform.cpp
    drawable.cpp
    timing.cpp
    texture.cpp
    text.cpp
    events.cpp
    errors.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

message(${CMAKE_CURRENT_SOURCE_DIR} "Current source directory")

target_include_directories(
    ${PROJECT_NAME} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../external/stb_image/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../../external/utf8cpp/include>
    $<INSTALL_INTERFACE:include>
)

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(LIBEVDEV REQUIRED libevdev)
    target_include_directories(${PROJECT_NAME} PUBLIC 
        $<BUILD_INTERFACE:${LIBEVDEV_INCLUDE_DIRS}>
        $<INSTALL_INTERFACE:${LIBEVDEV_INCLUDE_DIRS}>
    )
    message(STATUS "LIBEVDEV_INCLUDE_DIRS: ${LIBEVDEV_INCLUDE_DIRS}")
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LIBEVDEV_LIBRARIES})
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC OpenMP::OpenMP_CXX)

if (APPLE)
    find_library(COREFOUNDATION_FRAMEWORK CoreFoundation)
    find_library(IOKIT_FRAMEWORK IOKit)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${COREFOUNDATION_FRAMEWORK} ${IOKIT_FRAMEWORK})
endif()