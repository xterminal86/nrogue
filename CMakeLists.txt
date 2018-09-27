cmake_minimum_required(VERSION 2.6)
set (TARGET_NAME nrogue)
project (${TARGET_NAME})

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${TARGET_NAME} ${SOURCES})

target_link_libraries(${TARGET_NAME} ncurses)

