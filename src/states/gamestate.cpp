#include "gamestate.h"

int GameState::GetKeyDown()
{
#ifdef USE_SDL
  SDL_Event event;
  SDL_PollEvent(&event);
  if (event.type == SDL_KEYDOWN)
  {
    return event.key.keysym.sym;
  }
#else
  return getch();
#endif

  return 0;
}
