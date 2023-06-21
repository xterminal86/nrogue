cmake_minimum_required(VERSION 2.9)
set (TARGET_NAME dg-tunneler)
set (CMAKE_CXX_STANDARD 14)
project (${TARGET_NAME})

# ==============================================================================

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/components)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/levels)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/states)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/singletons)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/extra)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/ai)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/ai/types)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/dungeon-generators)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/behaviour-tree)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../src/behaviour-tree/tasks)

# ==============================================================================

file(GLOB SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/components/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/levels/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/states/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/singletons/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/extra/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ai/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ai/types/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/dungeon-generators/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/behaviour-tree/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/behaviour-tree/tasks/*.cpp

  # ----------------------------------------------------------------------------

  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/components/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/levels/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/states/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/singletons/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/extra/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ai/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/ai/types/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/dungeon-generators/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/behaviour-tree/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../../src/behaviour-tree/tasks/*.h
)

# ==============================================================================

list(REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/states/dev-console.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/states/dev-console.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/singletons/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/singletons/logger.h)

add_executable(${TARGET_NAME} ${SOURCES})

# ==============================================================================

if (USE_SDL)
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})

  if (WIN32)
    target_link_libraries(${TARGET_NAME} ${MINGW32_LIBRARY}
                                         ${SDL2MAIN_LIBRARY}
                                         ${SDL2_LIBRARY})
  else()
    target_link_libraries(${TARGET_NAME} SDL2)
  endif()

else()

  if (WIN32)
    target_link_libraries(${TARGET_NAME} pdcurses)
  else()
    find_package(Curses REQUIRED)
    target_link_libraries(${TARGET_NAME} ncurses)
  endif()

endif()
