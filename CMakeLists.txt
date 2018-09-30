cmake_minimum_required(VERSION 2.6)
set (TARGET_NAME nrogue)
project (${TARGET_NAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h)

add_executable(${TARGET_NAME} ${SOURCES})

if (WIN32)
  target_link_libraries(${TARGET_NAME} pdcurses)
else()
  target_link_libraries(${TARGET_NAME} ncurses)
endif()

