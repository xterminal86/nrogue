cmake_minimum_required(VERSION 2.9)
set (TARGET_NAME nrogue-tests)
set (CMAKE_CXX_STANDARD 14)
project (${TARGET_NAME})

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../components)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../levels)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../states)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../singletons)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../extra)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ai)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../ai/types)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../dungeon-generators)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../behaviour-tree)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../behaviour-tree/tasks)

file(GLOB SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../components/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../levels/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../states/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../singletons/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../core/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../extra/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../ai/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../ai/types/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../dungeon-generators/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../behaviour-tree/*.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../behaviour-tree/tasks/*.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/../components/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../levels/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../states/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../singletons/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../core/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../extra/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../ai/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../ai/types/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../dungeon-generators/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../behaviour-tree/*.h
  ${CMAKE_CURRENT_SOURCE_DIR}/../behaviour-tree/tasks/*.h
)

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  list(REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../states/dev-console.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../states/dev-console.h)
endif()

add_executable(${TARGET_NAME} ${SOURCES})

if (USE_SDL)
  if (WIN32)
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(${TARGET_NAME} ${MINGW32_LIBRARY}
                                           ${SDL2MAIN_LIBRARY}
                                            ${SDL2_LIBRARY})
  else()
    find_package(SDL2 REQUIRED)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(${TARGET_NAME} SDL2)
  endif()
else()
  if (WIN32)
    target_link_libraries(${TARGET_NAME} pdcurses)
  else()
    target_link_libraries(${TARGET_NAME} ncurses)
  endif()
endif()
