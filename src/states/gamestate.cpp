#include "gamestate.h"

int GameState::GetKeyDown()
{
  int res = -1;

#ifdef USE_SDL
  SDL_Event event;
  SDL_PollEvent(&event);

  static bool shiftPressed = false;
  static char passThrough = 0;

  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
    {
      shiftPressed = true;
    }
    else if (ShouldPassThrough(event.key.keysym.scancode))
    {
      res = event.key.keysym.sym;
    }
    else
    {
      // operator [] doesn't throw exception
      char c = _charsByScancodes[event.key.keysym.scancode];

      if (shiftPressed)
      {
        // printf("Shift + %c (%u;%i)\n", c, event.key.keysym.sym, (int)event.key.keysym.scancode);

        if (c == '2')
        {
          c = '@';
        }
        else if (c == '4')
        {
          c = '$';
        }
        else if (c == '/')
        {
          c = '?';
        }
        else if (c == '.')
        {
          c = '>';
        }
        else if (c == ',')
        {
          c = '<';
        }

        res = c;
      }
      else
      {
        // printf("%s (%u:%i)\n", SDL_GetKeyName(event.key.keysym.sym), event.key.keysym.sym, (int)event.key.keysym.scancode);

        char lowerCase = std::tolower(c);
        res = lowerCase;
      }
    }
  }
  else if (event.type == SDL_KEYUP)
  {
    if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT)
    {
      shiftPressed = false;
    }
  }

#else
  return getch();
#endif

  return res;
}

#ifdef USE_SDL
bool GameState::ShouldPassThrough(SDL_Scancode& sc)
{  
  auto res = std::find(_specialKeys.begin(), _specialKeys.end(), sc);
  return (res != _specialKeys.end());
}
#endif
