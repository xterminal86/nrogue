cmake_minimum_required(VERSION 2.6)
set (TARGET_NAME nrogue)
project (${TARGET_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=return-type")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/components)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/levels)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/states)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/singletons)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/extra)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/core)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/ai)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc/dungeon-generators)

file(GLOB SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp                  
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/components/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/levels/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/states/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/singletons/*.cpp                  
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/core/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/ai/*.cpp
                  ${CMAKE_CURRENT_SOURCE_DIR}/src/dungeon-generators/*.cpp

                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/components/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/levels/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/states/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/singletons/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/core/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/extra/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/ai/*.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/inc/dungeon-generators/*.h)

add_executable(${TARGET_NAME} ${SOURCES})

if (WIN32)
  target_link_libraries(${TARGET_NAME} pdcurses)
else()
  target_link_libraries(${TARGET_NAME} ncurses)
endif()

