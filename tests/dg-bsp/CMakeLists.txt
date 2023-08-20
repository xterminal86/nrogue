set (TARGET_NAME dg-bsp)
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
