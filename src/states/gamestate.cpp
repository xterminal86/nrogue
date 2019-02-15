#include "gamestate.h"

int GameState::GetKeyDown()
{
#ifdef USE_SDL
  SDL_Event event;
  SDL_PollEvent(&event);

  int res = -1;

  static Uint32 now = SDL_GetTicks();
  static Uint32 msPassed = now;
  static bool pressed = false;
  static bool shiftPressed = false;

  if (event.type == SDL_KEYDOWN)
  {
    std::string keyName = SDL_GetKeyName(event.key.keysym.sym);
    if (keyName == "Left Shift")
    {
      shiftPressed = true;
    }
    else if (ShouldPassThrough(keyName))
    {
      res = event.key.keysym.sym;
    }
    else
    {
      char c = keyName[0];

      //printf("Shift + %c\n", c);

      if (shiftPressed)
      {
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
        //printf("%s\n", keyName.data());
        char lowerCase = std::tolower(c);
        res = lowerCase;
      }
    }
  }
  else if (event.type == SDL_KEYUP)
  {
    std::string keyName = SDL_GetKeyName(event.key.keysym.sym);
    if (keyName == "Left Shift")
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
bool GameState::ShouldPassThrough(std::string& keyName)
{
  auto res = std::find(_validKeys.begin(), _validKeys.end(), keyName);
  return (res != _validKeys.end());
}
#endif
