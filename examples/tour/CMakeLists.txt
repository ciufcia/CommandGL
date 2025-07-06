add_executable(setup setup/setup.cpp)
target_link_libraries(setup PRIVATE CommandGL)

add_executable(console console/console.cpp)
target_link_libraries(console PRIVATE CommandGL)

add_executable(events events/events.cpp)
target_link_libraries(events PRIVATE CommandGL)

add_executable(drawing drawing/drawing.cpp)
target_link_libraries(drawing PRIVATE CommandGL)