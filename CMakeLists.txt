cmake_minimum_required(VERSION 2.9)
set (TARGET_NAME nrogue)
project (${TARGET_NAME})

if (MSVC)
  add_compile_options(/W1)
  add_definitions(-DMSVC_COMPILER)
  add_definitions(-D__PRETTY_FUNCTION__=__FUNCSIG__)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
else()
  add_compile_options(-Wall -Wno-switch -Wno-format -Werror=return-type -Wfatal-errors)
endif()

option(USE_SDL "Build with SDL2" false)
option(RUN_TESTS "Run some tests" false)

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  add_definitions(-DRELEASE_BUILD)
else()
  add_definitions(-DDEBUG_BUILD)
endif()

if (USE_SDL)
  add_definitions(-DUSE_SDL)
endif()

if (RUN_TESTS)
  add_definitions(-DRUN_TESTS)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/components)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/levels)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/states)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/singletons)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/extra)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/ai)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/ai/types)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/dungeon-generators)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/behaviour-tree)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/behaviour-tree/tasks)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/components/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/levels/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/singletons/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/extra/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/ai/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/ai/types/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/dungeon-generators/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/behaviour-tree/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/behaviour-tree/tasks/*.cpp

                  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/components/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/levels/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/singletons/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/extra/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/ai/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/ai/types/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/dungeon-generators/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/behaviour-tree/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/behaviour-tree/tasks/*.h)

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  list(REMOVE_ITEM SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/states/dev-console.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/states/dev-console.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/extra/tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/extra/tests.h)
endif()

add_executable(${TARGET_NAME} ${SOURCES})

set_target_properties(${TARGET_NAME} PROPERTIES CXX_STANDARD_REQUIRED 11)

if (WIN32)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
  message(STATUS, "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")
endif()

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


