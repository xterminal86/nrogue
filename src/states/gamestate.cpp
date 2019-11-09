#include "gamestate.h"
#include "printer.h"

int GameState::GetKeyDown(bool waitForEvent)
{  
#ifdef USE_SDL
  int res = -1;

  SDL_Event event;

  if (waitForEvent)
  {
    if (!SDL_PollEvent(&event))
    {
      return res;
    }
  }
  else
  {
    SDL_PollEvent(&event);
  }

  static bool shiftPressed = false;
  static char passThrough = 0;

  if (event.type == SDL_KEYDOWN)
  {
    if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT
     || event.key.keysym.scancode == SDL_SCANCODE_RSHIFT)
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
    if (event.key.keysym.scancode == SDL_SCANCODE_LSHIFT
     || event.key.keysym.scancode == SDL_SCANCODE_RSHIFT)
    {
      shiftPressed = false;
    }
  }

  return res;

#else
  return getch();
#endif
}

void GameState::DrawHeader(const std::string& header)
{
  int tw = Printer::Instance().TerminalWidth;

  for (int x = 0; x < tw; x++)
  {
    #ifdef USE_SDL
    Printer::Instance().PrintFB(x, 0, (int)NameCP437::HBAR_2, "#FFFFFF");
    #else
    Printer::Instance().PrintFB(x, 0, '-', "#FFFFFF");
    #endif
  }

  Printer::Instance().PrintFB(tw / 2, 0, header, Printer::kAlignCenter, "#FFFFFF", GlobalConstants::MessageBoxHeaderBgColor);
}

#ifdef USE_SDL
bool GameState::ShouldPassThrough(SDL_Scancode& sc)
{  
  auto res = std::find(_specialKeys.begin(), _specialKeys.end(), sc);
  return (res != _specialKeys.end());
}
#endif
