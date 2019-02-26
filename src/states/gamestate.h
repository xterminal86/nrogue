#ifndef GAMESTATE_H
#define GAMESTATE_H

#ifndef USE_SDL
#include <ncurses.h>
#else
#include <SDL2/SDL.h>
#include <string>
#include <algorithm>

#include "constants.h"
#endif

#include <typeinfo>

class GameState
{
  public:
    virtual ~GameState() {}

    virtual void Init() {}
    virtual void Cleanup() {}
    virtual void Prepare() {}
    virtual void HandleInput() = 0;
    virtual void Update(bool forceUpdate = false) = 0;

    int GetKeyDown();

  protected:
    int _keyPressed;    

    void DrawHeader(const std::string& header);

#ifdef USE_SDL
    bool ShouldPassThrough(SDL_Scancode& sc);

    std::map<SDL_Scancode, char> _charsByScancodes =
    {
      { SDL_SCANCODE_GRAVE, '`' },
      { SDL_SCANCODE_1, '1' },
      { SDL_SCANCODE_2, '2' },
      { SDL_SCANCODE_3, '3' },
      { SDL_SCANCODE_4, '4' },
      { SDL_SCANCODE_5, '5' },
      { SDL_SCANCODE_6, '6' },
      { SDL_SCANCODE_7, '7' },
      { SDL_SCANCODE_8, '8' },
      { SDL_SCANCODE_9, '9' },
      { SDL_SCANCODE_0, '0' },
      { SDL_SCANCODE_MINUS, '-' },
      { SDL_SCANCODE_EQUALS, '=' },
      { SDL_SCANCODE_Q, 'Q' },
      { SDL_SCANCODE_W, 'W' },
      { SDL_SCANCODE_E, 'E' },
      { SDL_SCANCODE_R, 'R' },
      { SDL_SCANCODE_T, 'T' },
      { SDL_SCANCODE_Y, 'Y' },
      { SDL_SCANCODE_U, 'U' },
      { SDL_SCANCODE_I, 'I' },
      { SDL_SCANCODE_O, 'O' },
      { SDL_SCANCODE_P, 'P' },
      { SDL_SCANCODE_LEFTBRACKET, '[' },
      { SDL_SCANCODE_RIGHTBRACKET, ']' },
      { SDL_SCANCODE_A, 'A' },
      { SDL_SCANCODE_S, 'S' },
      { SDL_SCANCODE_D, 'D' },
      { SDL_SCANCODE_F, 'F' },
      { SDL_SCANCODE_G, 'G' },
      { SDL_SCANCODE_H, 'H' },
      { SDL_SCANCODE_J, 'J' },
      { SDL_SCANCODE_K, 'K' },
      { SDL_SCANCODE_L, 'L' },
      { SDL_SCANCODE_SEMICOLON, ';' },
      { SDL_SCANCODE_APOSTROPHE, '\'' },
      { SDL_SCANCODE_BACKSLASH, '\\' },
      { SDL_SCANCODE_Z, 'Z' },
      { SDL_SCANCODE_X, 'X' },
      { SDL_SCANCODE_C, 'C' },
      { SDL_SCANCODE_V, 'V' },
      { SDL_SCANCODE_B, 'B' },
      { SDL_SCANCODE_N, 'N' },
      { SDL_SCANCODE_M, 'M' },
      { SDL_SCANCODE_COMMA, ',' },
      { SDL_SCANCODE_PERIOD, '.' },
      { SDL_SCANCODE_SLASH, '/' }
    };

    std::vector<SDL_Scancode> _specialKeys =
    {
      SDL_SCANCODE_SPACE,
      SDL_SCANCODE_RETURN,
      SDL_SCANCODE_BACKSPACE,
      SDL_SCANCODE_KP_1,
      SDL_SCANCODE_KP_2,
      SDL_SCANCODE_KP_3,
      SDL_SCANCODE_KP_4,
      SDL_SCANCODE_KP_5,
      SDL_SCANCODE_KP_6,
      SDL_SCANCODE_KP_7,
      SDL_SCANCODE_KP_8,
      SDL_SCANCODE_KP_9,
      SDL_SCANCODE_UP,
      SDL_SCANCODE_DOWN,
      SDL_SCANCODE_LEFT,
      SDL_SCANCODE_RIGHT,
      SDL_SCANCODE_LCTRL
    };
#endif

};

#endif
