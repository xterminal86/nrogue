cmake_minimum_required(VERSION 2.9)
set (TARGET_NAME nrogue)
set (OBJECT_LIB_NAME ${TARGET_NAME}-objlib)
set (OBJLIB_LINK_NAME $<TARGET_OBJECTS:${OBJECT_LIB_NAME}>)
set (CMAKE_CXX_STANDARD 14)
project (${TARGET_NAME})

# ==============================================================================

if (MSVC)
  add_compile_options(/W1)
  add_definitions(-DMSVC_COMPILER)
  add_definitions(-D__PRETTY_FUNCTION__=__FUNCSIG__)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:WINDOWS")
else()
  add_compile_options(-Wall
                      #-Wextra
                      -Wno-switch
                      -Wno-format
                      -Werror=return-type
                      -Wfatal-errors)
endif()

# ==============================================================================

option(USE_SDL "Build with SDL2" false)
option(BUILD_TESTS "Allow various test executables appear in targets list" false)

if (${CMAKE_BUILD_TYPE} MATCHES Release)
  add_definitions(-DRELEASE_BUILD)
else()
  add_definitions(-DDEBUG_BUILD)
endif()

if (USE_SDL)
  add_definitions(-DUSE_SDL)
endif()

if (BUILD_TESTS)
  add_definitions(-DBUILD_TESTS)
endif()

# ==============================================================================

set(BUILD_VERSION_TEXT ${BUILD_VERSION_TEXT})
configure_file(version-config.h.in
                ${CMAKE_CURRENT_SOURCE_DIR}/src/extra/version-config.h
                @ONLY)

# ==============================================================================

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

file(GLOB SOURCES
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

  # ----------------------------------------------------------------------------

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
    ${CMAKE_CURRENT_SOURCE_DIR}/src/singletons/logger.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/singletons/logger.h)
endif()

#
# To allow sharing of object files since it's all the same shit anyway.
#
add_library(${OBJECT_LIB_NAME} OBJECT ${SOURCES})

# ==============================================================================

if (BUILD_TESTS)
  add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests)
endif()

# ==============================================================================

add_executable(${TARGET_NAME}
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${OBJLIB_LINK_NAME})

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
