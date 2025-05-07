set (TARGET_NAME rooms-transform)
project (${TARGET_NAME})

# ==============================================================================

add_executable(${TARGET_NAME} main.cpp ${OBJLIB_LINK_NAME})

# ==============================================================================

if (USE_SDL)
  find_package(SDL2 REQUIRED)
  include_directories(${SDL2_INCLUDE_DIRS})

  if (WIN32)
    target_link_libraries(${TARGET_NAME} ${MINGW32_LIBRARY}
                                         ${SDL2MAIN_LIBRARY}
                                         ${SDL2_LIBRARY})
  else()
    target_link_libraries(${TARGET_NAME} ${SDL2_LIBRARY})
  endif()

else()
  find_package(Curses REQUIRED)
  include_directories(${CURSES_INCLUDE_DIRS})
  target_link_libraries(${TARGET_NAME} ${CURSES_LIBRARIES})
endif()
